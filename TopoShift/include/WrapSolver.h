#pragma once

#include "Math.h"
#include "LandmarkManager.h"
#include <vector>

namespace TopoShift {

class Mesh;

// Wrap solver configuration
struct WrapConfig {
    float landmarkWeight;        // Weight for landmark constraints (0-100)
    float landmarkFalloffRadius; // Falloff radius in world units
    bool pinnedLandmarks;        // Lock landmark vertices (very high weight)
    int iterations;              // Number of solver iterations
    bool useShrinkwrap;          // Apply shrinkwrap refinement pass

    WrapConfig()
        : landmarkWeight(25.0f), landmarkFalloffRadius(0.1f),
          pinnedLandmarks(false), iterations(50), useShrinkwrap(true) {}
};

// Wrap solver result
struct WrapResult {
    bool success;
    float finalEnergy;
    int iterations;

    WrapResult() : success(false), finalEnergy(0.0f), iterations(0) {}
};

class WrapSolver {
public:
    WrapSolver();
    ~WrapSolver() = default;

    // Run wrap solve
    WrapResult solve(Mesh* targetMesh, const Mesh* sourceMesh,
                     const LandmarkManager& landmarkManager,
                     const WrapConfig& config);

    // Get progress (0-1) for UI
    float getProgress() const { return m_progress; }

    // Cancel ongoing solve
    void cancel() { m_cancelled = true; }

private:
    float m_progress;
    bool m_cancelled;

    // Compute landmark weights for each vertex
    std::vector<float> computeLandmarkWeights(const Mesh* targetMesh,
                                               const LandmarkManager& landmarkManager,
                                               const WrapConfig& config);

    // Compute falloff weight based on distance
    float computeFalloff(float distance, float radius);

    // Perform one solver iteration
    float solveIteration(Mesh* targetMesh, const Mesh* sourceMesh,
                         const LandmarkManager& landmarkManager,
                         const std::vector<float>& weights,
                         const WrapConfig& config);

    // Apply shrinkwrap pass
    void applyShrinkwrap(Mesh* targetMesh, const Mesh* sourceMesh);

    // Project vertex onto source mesh
    Vec3 projectToMesh(const Vec3& vertex, const Mesh* sourceMesh);
};

} // namespace TopoShift
