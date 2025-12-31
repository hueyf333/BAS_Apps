#include "LandmarkManager.h"
#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace TopoShift {

const char* getLandmarkName(LandmarkType type) {
    switch (type) {
        case LandmarkType::LeftEyeCornerInner: return "LeftEyeCornerInner";
        case LandmarkType::LeftEyeCornerOuter: return "LeftEyeCornerOuter";
        case LandmarkType::RightEyeCornerInner: return "RightEyeCornerInner";
        case LandmarkType::RightEyeCornerOuter: return "RightEyeCornerOuter";
        case LandmarkType::NoseTip: return "NoseTip";
        case LandmarkType::MouthCornerLeft: return "MouthCornerLeft";
        case LandmarkType::MouthCornerRight: return "MouthCornerRight";
        case LandmarkType::ChinTip: return "ChinTip";
        case LandmarkType::BrowLeftCenter: return "BrowLeftCenter";
        case LandmarkType::BrowRightCenter: return "BrowRightCenter";
        default: return "Unknown";
    }
}

int getLandmarkCount() {
    return static_cast<int>(LandmarkType::COUNT);
}

LandmarkManager::LandmarkManager()
    : m_sourceMesh(nullptr), m_targetMesh(nullptr) {
    // Initialize all landmark pairs
    int count = getLandmarkCount();
    m_landmarkPairs.resize(count);
    for (int i = 0; i < count; ++i) {
        m_landmarkPairs[i].sourceRef.type = static_cast<LandmarkType>(i);
        m_landmarkPairs[i].targetRef.type = static_cast<LandmarkType>(i);
        m_landmarkPairs[i].sourceRef.meshId = 0;
        m_landmarkPairs[i].targetRef.meshId = 1;
    }
}

void LandmarkManager::initialize(Mesh* sourceMesh, Mesh* targetMesh) {
    m_sourceMesh = sourceMesh;
    m_targetMesh = targetMesh;
}

bool LandmarkManager::setLandmarkFromPick(LandmarkType type, int meshId, const Ray& ray) {
    Mesh* mesh = getMesh(meshId);
    if (!mesh) return false;

    // Raycast against mesh
    Mesh::RayHit hit = mesh->raycast(ray);
    if (!hit.hit) return false;

    // Get landmark ref
    LandmarkRef& ref = (meshId == 0) ? m_landmarkPairs[static_cast<int>(type)].sourceRef
                                      : m_landmarkPairs[static_cast<int>(type)].targetRef;

    // Store as triangle + barycentric
    ref.meshId = meshId;
    ref.mode = LandmarkMode::TriangleBary;
    ref.triangleIndex = hit.triangleIndex;
    ref.barycentric = Vec3(1.0f - hit.u - hit.v, hit.u, hit.v);
    ref.worldPosition = hit.position;
    ref.isSet = true;

    return true;
}

void LandmarkManager::clearLandmark(LandmarkType type, int meshId) {
    LandmarkRef& ref = (meshId == 0) ? m_landmarkPairs[static_cast<int>(type)].sourceRef
                                      : m_landmarkPairs[static_cast<int>(type)].targetRef;
    ref.isSet = false;
}

void LandmarkManager::clearAll() {
    for (auto& pair : m_landmarkPairs) {
        pair.sourceRef.isSet = false;
        pair.targetRef.isSet = false;
    }
}

LandmarkPair& LandmarkManager::getLandmarkPair(LandmarkType type) {
    return m_landmarkPairs[static_cast<int>(type)];
}

const LandmarkPair& LandmarkManager::getLandmarkPair(LandmarkType type) const {
    return m_landmarkPairs[static_cast<int>(type)];
}

std::vector<LandmarkPair> LandmarkManager::getCompletePairs() const {
    std::vector<LandmarkPair> complete;
    for (const auto& pair : m_landmarkPairs) {
        if (pair.isComplete() && pair.enabled) {
            complete.push_back(pair);
        }
    }
    return complete;
}

void LandmarkManager::setEnabled(LandmarkType type, bool enabled) {
    m_landmarkPairs[static_cast<int>(type)].enabled = enabled;
}

std::string LandmarkManager::toJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"source_mesh\": \"" << (m_sourceMesh ? m_sourceMesh->getFilename() : "") << "\",\n";
    ss << "  \"target_mesh\": \"" << (m_targetMesh ? m_targetMesh->getFilename() : "") << "\",\n";
    ss << "  \"landmarks\": [\n";

    bool first = true;
    for (const auto& pair : m_landmarkPairs) {
        if (!pair.isComplete()) continue;

        if (!first) ss << ",\n";
        first = false;

        ss << "    {\n";
        ss << "      \"type\": \"" << getLandmarkName(pair.sourceRef.type) << "\",\n";
        ss << "      \"source\": {\n";
        ss << "        \"mode\": \"" << (pair.sourceRef.mode == LandmarkMode::Vertex ? "vertex" : "triangle") << "\",\n";
        if (pair.sourceRef.mode == LandmarkMode::Vertex) {
            ss << "        \"vertex_index\": " << pair.sourceRef.vertexIndex << ",\n";
        } else {
            ss << "        \"triangle_index\": " << pair.sourceRef.triangleIndex << ",\n";
            ss << "        \"barycentric\": [" << pair.sourceRef.barycentric.x << ", "
               << pair.sourceRef.barycentric.y << ", " << pair.sourceRef.barycentric.z << "],\n";
        }
        ss << "        \"world_position\": [" << pair.sourceRef.worldPosition.x << ", "
           << pair.sourceRef.worldPosition.y << ", " << pair.sourceRef.worldPosition.z << "]\n";
        ss << "      },\n";
        ss << "      \"target\": {\n";
        ss << "        \"mode\": \"" << (pair.targetRef.mode == LandmarkMode::Vertex ? "vertex" : "triangle") << "\",\n";
        if (pair.targetRef.mode == LandmarkMode::Vertex) {
            ss << "        \"vertex_index\": " << pair.targetRef.vertexIndex << ",\n";
        } else {
            ss << "        \"triangle_index\": " << pair.targetRef.triangleIndex << ",\n";
            ss << "        \"barycentric\": [" << pair.targetRef.barycentric.x << ", "
               << pair.targetRef.barycentric.y << ", " << pair.targetRef.barycentric.z << "],\n";
        }
        ss << "        \"world_position\": [" << pair.targetRef.worldPosition.x << ", "
           << pair.targetRef.worldPosition.y << ", " << pair.targetRef.worldPosition.z << "]\n";
        ss << "      },\n";
        ss << "      \"enabled\": " << (pair.enabled ? "true" : "false") << "\n";
        ss << "    }";
    }

    ss << "\n  ]\n";
    ss << "}\n";

    return ss.str();
}

bool LandmarkManager::saveToJSON(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << toJSON();
    return true;
}

bool LandmarkManager::loadFromJSON(const std::string& filename) {
    // Simplified: not implementing full JSON parsing for brevity
    // In a real implementation, use a JSON library like nlohmann/json
    return false;
}

bool LandmarkManager::fromJSON(const std::string& json) {
    // Simplified: not implementing full JSON parsing for brevity
    return false;
}

void LandmarkManager::snapToNearestVertex(LandmarkType type) {
    LandmarkRef& targetRef = m_landmarkPairs[static_cast<int>(type)].targetRef;
    if (!targetRef.isSet || !m_targetMesh) return;

    int nearestIdx = m_targetMesh->findNearestVertex(targetRef.worldPosition);
    if (nearestIdx >= 0) {
        targetRef.mode = LandmarkMode::Vertex;
        targetRef.vertexIndex = nearestIdx;
        targetRef.worldPosition = m_targetMesh->getVertexPosition(nearestIdx);
    }
}

void LandmarkManager::mirrorLandmark(LandmarkType type) {
    // Simplified: implement L/R mirroring for symmetric landmarks
    // This would swap landmarks and mirror their positions across X axis
}

void LandmarkManager::updateWorldPositions() {
    for (auto& pair : m_landmarkPairs) {
        if (pair.sourceRef.isSet && m_sourceMesh) {
            if (pair.sourceRef.mode == LandmarkMode::Vertex) {
                pair.sourceRef.worldPosition = m_sourceMesh->getVertexPosition(pair.sourceRef.vertexIndex);
            } else {
                pair.sourceRef.worldPosition = m_sourceMesh->getPositionFromBary(
                    pair.sourceRef.triangleIndex, pair.sourceRef.barycentric);
            }
        }
        if (pair.targetRef.isSet && m_targetMesh) {
            if (pair.targetRef.mode == LandmarkMode::Vertex) {
                pair.targetRef.worldPosition = m_targetMesh->getVertexPosition(pair.targetRef.vertexIndex);
            } else {
                pair.targetRef.worldPosition = m_targetMesh->getPositionFromBary(
                    pair.targetRef.triangleIndex, pair.targetRef.barycentric);
            }
        }
    }
}

int LandmarkManager::getSetCount(int meshId) const {
    int count = 0;
    for (const auto& pair : m_landmarkPairs) {
        const LandmarkRef& ref = (meshId == 0) ? pair.sourceRef : pair.targetRef;
        if (ref.isSet) count++;
    }
    return count;
}

int LandmarkManager::getCompleteCount() const {
    int count = 0;
    for (const auto& pair : m_landmarkPairs) {
        if (pair.isComplete()) count++;
    }
    return count;
}

Mesh* LandmarkManager::getMesh(int meshId) {
    return (meshId == 0) ? m_sourceMesh : m_targetMesh;
}

const Mesh* LandmarkManager::getMesh(int meshId) const {
    return (meshId == 0) ? m_sourceMesh : m_targetMesh;
}

std::string LandmarkManager::getLandmarkTypeName(LandmarkType type) const {
    return getLandmarkName(type);
}

LandmarkType LandmarkManager::parseLandmarkType(const std::string& name) const {
    for (int i = 0; i < getLandmarkCount(); ++i) {
        if (name == getLandmarkName(static_cast<LandmarkType>(i))) {
            return static_cast<LandmarkType>(i);
        }
    }
    return LandmarkType::LeftEyeCornerInner;
}

} // namespace TopoShift
