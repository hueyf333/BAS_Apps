#pragma once

#include "Math.h"
#include <string>
#include <vector>
#include <map>
#include <optional>

namespace TopoShift {

// Landmark types (semantic locations)
enum class LandmarkType {
    LeftEyeCornerInner,
    LeftEyeCornerOuter,
    RightEyeCornerInner,
    RightEyeCornerOuter,
    NoseTip,
    MouthCornerLeft,
    MouthCornerRight,
    ChinTip,
    BrowLeftCenter,
    BrowRightCenter,
    COUNT
};

const char* getLandmarkName(LandmarkType type);
int getLandmarkCount();

// Landmark reference mode
enum class LandmarkMode {
    Vertex,        // Reference by vertex index
    TriangleBary   // Reference by triangle + barycentric coords
};

// Reference to a landmark on a mesh
struct LandmarkRef {
    int meshId;                     // 0 = source, 1 = target
    LandmarkType type;
    LandmarkMode mode;
    
    // For Vertex mode
    int vertexIndex;
    
    // For TriangleBary mode
    int triangleIndex;
    Vec3 barycentric;
    
    // Cached world position
    Vec3 worldPosition;
    
    bool isSet;

    LandmarkRef()
        : meshId(-1), type(LandmarkType::LeftEyeCornerInner),
          mode(LandmarkMode::TriangleBary), vertexIndex(-1),
          triangleIndex(-1), barycentric(0, 0, 0),
          worldPosition(0, 0, 0), isSet(false) {}
};

// Pair of source and target landmarks
struct LandmarkPair {
    LandmarkRef sourceRef;
    LandmarkRef targetRef;
    bool enabled;

    LandmarkPair() : enabled(true) {}

    bool isComplete() const {
        return sourceRef.isSet && targetRef.isSet;
    }
};

class Mesh;

// Manages all landmarks for the application
class LandmarkManager {
public:
    LandmarkManager();
    ~LandmarkManager() = default;

    // Initialize with source and target meshes
    void initialize(Mesh* sourceMesh, Mesh* targetMesh);

    // Set a landmark from a ray pick
    bool setLandmarkFromPick(LandmarkType type, int meshId, const Ray& ray);

    // Clear a specific landmark
    void clearLandmark(LandmarkType type, int meshId);

    // Clear all landmarks
    void clearAll();

    // Get landmark pair
    LandmarkPair& getLandmarkPair(LandmarkType type);
    const LandmarkPair& getLandmarkPair(LandmarkType type) const;

    // Get all pairs
    std::vector<LandmarkPair>& getAllPairs() { return m_landmarkPairs; }
    const std::vector<LandmarkPair>& getAllPairs() const { return m_landmarkPairs; }

    // Get only complete pairs (both source and target set)
    std::vector<LandmarkPair> getCompletePairs() const;

    // Enable/disable a landmark pair
    void setEnabled(LandmarkType type, bool enabled);

    // Save/load landmarks to/from JSON
    bool saveToJSON(const std::string& filename) const;
    bool loadFromJSON(const std::string& filename);

    // Get JSON string for current landmarks
    std::string toJSON() const;
    bool fromJSON(const std::string& json);

    // Snap target landmark to nearest vertex (optional feature)
    void snapToNearestVertex(LandmarkType type);

    // Mirror landmark from left to right or vice versa (optional feature)
    void mirrorLandmark(LandmarkType type);

    // Update world positions from mesh (call after mesh transform)
    void updateWorldPositions();

    // Get statistics
    int getSetCount(int meshId) const;
    int getCompleteCount() const;

private:
    Mesh* m_sourceMesh;
    Mesh* m_targetMesh;
    std::vector<LandmarkPair> m_landmarkPairs;

    // Helper to get mesh by id
    Mesh* getMesh(int meshId);
    const Mesh* getMesh(int meshId) const;

    // Helper to get landmark type name
    std::string getLandmarkTypeName(LandmarkType type) const;
    LandmarkType parseLandmarkType(const std::string& name) const;
};

} // namespace TopoShift
