#pragma once

#include "Math.h"
#include <vector>
#include <string>
#include <memory>

namespace TopoShift {

// Simple BVH node for raycasting
struct BVHNode {
    AABB bounds;
    int leftChild;    // -1 if leaf
    int rightChild;   // -1 if leaf
    int startTri;     // Start triangle index (if leaf)
    int triCount;     // Number of triangles (if leaf)
};

class Mesh {
public:
    Mesh();
    ~Mesh() = default;

    // Load mesh from OBJ file
    bool loadFromOBJ(const std::string& filename);

    // Save mesh to OBJ file
    bool saveToOBJ(const std::string& filename) const;

    // Access mesh data
    const std::vector<Vec3>& getVertices() const { return m_vertices; }
    const std::vector<Vec3>& getNormals() const { return m_normals; }
    const std::vector<uint32_t>& getIndices() const { return m_indices; }
    
    std::vector<Vec3>& getVertices() { return m_vertices; }
    std::vector<Vec3>& getNormals() { return m_normals; }

    // Get triangle vertices
    void getTriangle(int triIndex, Vec3& v0, Vec3& v1, Vec3& v2) const;

    // Raycast against mesh
    struct RayHit {
        bool hit;
        int triangleIndex;
        float t;           // Ray parameter
        float u, v;        // Barycentric u, v (w = 1 - u - v)
        Vec3 position;     // World hit position
        Vec3 normal;       // Interpolated normal

        RayHit() : hit(false), triangleIndex(-1), t(FLT_MAX), u(0), v(0) {}
    };
    
    RayHit raycast(const Ray& ray) const;

    // Apply transform to all vertices
    void applyTransform(const Transform& transform);

    // Compute normals
    void computeNormals();

    // Build BVH for fast raycasting
    void buildBVH();

    // Get bounding box
    AABB getBoundingBox() const;

    // Get world position from vertex index
    Vec3 getVertexPosition(int vertexIndex) const;

    // Get world position from triangle + barycentric
    Vec3 getPositionFromBary(int triangleIndex, const Vec3& bary) const;

    // Find nearest vertex to a point
    int findNearestVertex(const Vec3& point) const;

    // Get vertex count
    size_t getVertexCount() const { return m_vertices.size(); }
    size_t getTriangleCount() const { return m_indices.size() / 3; }

    // Get/set transform
    const Transform& getTransform() const { return m_transform; }
    void setTransform(const Transform& transform) { m_transform = transform; }

    // Get filename
    const std::string& getFilename() const { return m_filename; }

private:
    std::vector<Vec3> m_vertices;
    std::vector<Vec3> m_normals;
    std::vector<uint32_t> m_indices;
    
    std::vector<BVHNode> m_bvhNodes;
    std::vector<int> m_triIndices; // Reordered triangle indices for BVH

    Transform m_transform;
    std::string m_filename;

    // BVH construction helpers
    void buildBVHRecursive(int nodeIndex, int startTri, int endTri);
    AABB computeTriangleBounds(int triIndex) const;
    
    // Raycast helpers
    bool raycastBVH(const Ray& ray, RayHit& hit, int nodeIndex) const;
};

} // namespace TopoShift
