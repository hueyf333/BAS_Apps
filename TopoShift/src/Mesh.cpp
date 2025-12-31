#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace TopoShift {

Mesh::Mesh() : m_transform() {}

bool Mesh::loadFromOBJ(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    m_filename = filename;
    m_vertices.clear();
    m_normals.clear();
    m_indices.clear();

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vec3 v;
            iss >> v.x >> v.y >> v.z;
            m_vertices.push_back(v);
        } else if (prefix == "vn") {
            Vec3 n;
            iss >> n.x >> n.y >> n.z;
            m_normals.push_back(n);
        } else if (prefix == "f") {
            // Simple triangle parsing (v or v/vt/vn format)
            std::vector<int> vIndices;
            std::string vertex;
            while (iss >> vertex) {
                size_t slash = vertex.find('/');
                int vIdx = std::stoi(slash != std::string::npos ? vertex.substr(0, slash) : vertex) - 1;
                vIndices.push_back(vIdx);
            }
            // Triangulate if needed (simple fan triangulation)
            for (size_t i = 1; i + 1 < vIndices.size(); ++i) {
                m_indices.push_back(vIndices[0]);
                m_indices.push_back(vIndices[i]);
                m_indices.push_back(vIndices[i + 1]);
            }
        }
    }

    // Compute normals if not provided
    if (m_normals.empty()) {
        computeNormals();
    }

    // Build BVH for raycasting
    buildBVH();

    return !m_vertices.empty();
}

bool Mesh::saveToOBJ(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    for (const auto& v : m_vertices) {
        file << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }

    for (const auto& n : m_normals) {
        file << "vn " << n.x << " " << n.y << " " << n.z << "\n";
    }

    for (size_t i = 0; i < m_indices.size(); i += 3) {
        file << "f " << (m_indices[i] + 1) << " "
             << (m_indices[i + 1] + 1) << " "
             << (m_indices[i + 2] + 1) << "\n";
    }

    return true;
}

void Mesh::getTriangle(int triIndex, Vec3& v0, Vec3& v1, Vec3& v2) const {
    int idx = triIndex * 3;
    v0 = m_vertices[m_indices[idx]];
    v1 = m_vertices[m_indices[idx + 1]];
    v2 = m_vertices[m_indices[idx + 2]];
}

Mesh::RayHit Mesh::raycast(const Ray& ray) const {
    RayHit result;
    if (m_bvhNodes.empty()) {
        // Brute force if no BVH
        for (size_t i = 0; i < m_indices.size(); i += 3) {
            Vec3 v0 = m_vertices[m_indices[i]];
            Vec3 v1 = m_vertices[m_indices[i + 1]];
            Vec3 v2 = m_vertices[m_indices[i + 2]];
            
            float t, u, v;
            if (intersectTriangle(ray, v0, v1, v2, t, u, v)) {
                if (t < result.t) {
                    result.hit = true;
                    result.triangleIndex = static_cast<int>(i / 3);
                    result.t = t;
                    result.u = u;
                    result.v = v;
                    result.position = ray.at(t);
                }
            }
        }
    } else {
        raycastBVH(ray, result, 0);
    }

    return result;
}

void Mesh::applyTransform(const Transform& transform) {
    XMMATRIX mat = transform.toMatrix();
    for (auto& v : m_vertices) {
        XMVECTOR vp = XMVectorSet(v.x, v.y, v.z, 1.0f);
        XMVECTOR result = XMVector3Transform(vp, mat);
        XMFLOAT3 fresult;
        XMStoreFloat3(&fresult, result);
        v = Vec3(fresult);
    }
    computeNormals();
}

void Mesh::computeNormals() {
    m_normals.resize(m_vertices.size(), Vec3(0, 0, 0));

    for (size_t i = 0; i < m_indices.size(); i += 3) {
        Vec3 v0 = m_vertices[m_indices[i]];
        Vec3 v1 = m_vertices[m_indices[i + 1]];
        Vec3 v2 = m_vertices[m_indices[i + 2]];

        Vec3 normal = (v1 - v0).cross(v2 - v0).normalized();

        m_normals[m_indices[i]] += normal;
        m_normals[m_indices[i + 1]] += normal;
        m_normals[m_indices[i + 2]] += normal;
    }

    for (auto& n : m_normals) {
        n = n.normalized();
    }
}

void Mesh::buildBVH() {
    if (m_indices.empty()) return;

    int triCount = static_cast<int>(m_indices.size() / 3);
    m_triIndices.resize(triCount);
    for (int i = 0; i < triCount; ++i) {
        m_triIndices[i] = i;
    }

    m_bvhNodes.reserve(triCount * 2);
    m_bvhNodes.clear();
    m_bvhNodes.push_back(BVHNode());
    buildBVHRecursive(0, 0, triCount);
}

AABB Mesh::getBoundingBox() const {
    AABB bounds;
    for (const auto& v : m_vertices) {
        bounds.expand(v);
    }
    return bounds;
}

Vec3 Mesh::getVertexPosition(int vertexIndex) const {
    if (vertexIndex >= 0 && vertexIndex < static_cast<int>(m_vertices.size())) {
        return m_vertices[vertexIndex];
    }
    return Vec3(0, 0, 0);
}

Vec3 Mesh::getPositionFromBary(int triangleIndex, const Vec3& bary) const {
    Vec3 v0, v1, v2;
    getTriangle(triangleIndex, v0, v1, v2);
    return barycentricInterpolate(bary, v0, v1, v2);
}

int Mesh::findNearestVertex(const Vec3& point) const {
    int nearest = -1;
    float minDist = FLT_MAX;

    for (size_t i = 0; i < m_vertices.size(); ++i) {
        float dist = (m_vertices[i] - point).lengthSquared();
        if (dist < minDist) {
            minDist = dist;
            nearest = static_cast<int>(i);
        }
    }

    return nearest;
}

void Mesh::buildBVHRecursive(int nodeIndex, int startTri, int endTri) {
    BVHNode& node = m_bvhNodes[nodeIndex];
    node.leftChild = -1;
    node.rightChild = -1;
    node.startTri = startTri;
    node.triCount = endTri - startTri;

    // Compute bounds
    for (int i = startTri; i < endTri; ++i) {
        node.bounds.expand(computeTriangleBounds(m_triIndices[i]));
    }

    // Leaf node if few triangles
    if (node.triCount <= 4) return;

    // Split along longest axis
    Vec3 extent = node.bounds.extents();
    int axis = (extent.x > extent.y) ? ((extent.x > extent.z) ? 0 : 2) : ((extent.y > extent.z) ? 1 : 2);

    // Sort triangles along axis
    std::sort(m_triIndices.begin() + startTri, m_triIndices.begin() + endTri,
              [this, axis](int a, int b) {
                  Vec3 centerA = computeTriangleBounds(a).center();
                  Vec3 centerB = computeTriangleBounds(b).center();
                  float ca = (axis == 0) ? centerA.x : ((axis == 1) ? centerA.y : centerA.z);
                  float cb = (axis == 0) ? centerB.x : ((axis == 1) ? centerB.y : centerB.z);
                  return ca < cb;
              });

    int mid = (startTri + endTri) / 2;

    node.leftChild = static_cast<int>(m_bvhNodes.size());
    m_bvhNodes.push_back(BVHNode());
    buildBVHRecursive(node.leftChild, startTri, mid);

    node.rightChild = static_cast<int>(m_bvhNodes.size());
    m_bvhNodes.push_back(BVHNode());
    buildBVHRecursive(node.rightChild, mid, endTri);
}

AABB Mesh::computeTriangleBounds(int triIndex) const {
    Vec3 v0, v1, v2;
    getTriangle(triIndex, v0, v1, v2);
    AABB bounds;
    bounds.expand(v0);
    bounds.expand(v1);
    bounds.expand(v2);
    return bounds;
}

bool Mesh::raycastBVH(const Ray& ray, RayHit& hit, int nodeIndex) const {
    const BVHNode& node = m_bvhNodes[nodeIndex];

    float tMin, tMax;
    if (!node.bounds.intersects(ray, tMin, tMax)) return false;
    if (tMin > hit.t) return false;

    if (node.leftChild < 0) {
        // Leaf node - test triangles
        bool hitAny = false;
        for (int i = 0; i < node.triCount; ++i) {
            int triIdx = m_triIndices[node.startTri + i];
            Vec3 v0, v1, v2;
            getTriangle(triIdx, v0, v1, v2);

            float t, u, v;
            if (intersectTriangle(ray, v0, v1, v2, t, u, v)) {
                if (t < hit.t) {
                    hit.hit = true;
                    hit.triangleIndex = triIdx;
                    hit.t = t;
                    hit.u = u;
                    hit.v = v;
                    hit.position = ray.at(t);
                    hitAny = true;
                }
            }
        }
        return hitAny;
    }

    // Inner node - recurse
    bool hitLeft = raycastBVH(ray, hit, node.leftChild);
    bool hitRight = raycastBVH(ray, hit, node.rightChild);
    return hitLeft || hitRight;
}

} // namespace TopoShift
