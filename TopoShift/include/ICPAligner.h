#pragma once

#include "Math.h"
#include "LandmarkManager.h"
#include <vector>

namespace TopoShift {

class Mesh;

// ICP Alignment configuration
struct ICPConfig {
    bool useLandmarks;           // Use landmarks for initial alignment
    bool useICPRefine;           // Use ICP refinement
    int icpIterations;           // Number of ICP iterations (1-30)
    int sampleCount;             // Number of sample points (100-5000)
    float distanceThreshold;     // Outlier rejection threshold
    bool allowUniformScale;      // Allow uniform scaling (Umeyama)
    bool applyToTarget;          // Apply transform to target mesh

    ICPConfig()
        : useLandmarks(true), useICPRefine(true),
          icpIterations(10), sampleCount(1000),
          distanceThreshold(0.1f), allowUniformScale(false),
          applyToTarget(true) {}
};

// ICP Alignment result
struct ICPResult {
    Transform transform;         // Computed transform (R, t, optional scale)
    float rmsLandmarkError;      // RMS error for landmarks
    float rmsICPResidual;        // RMS residual for ICP
    int correspondenceCount;     // Number of correspondences used
    bool success;                // Whether alignment succeeded

    ICPResult()
        : transform(), rmsLandmarkError(0.0f), rmsICPResidual(0.0f),
          correspondenceCount(0), success(false) {}
};

class ICPAligner {
public:
    ICPAligner();
    ~ICPAligner() = default;

    // Run alignment
    ICPResult align(const Mesh* sourceMesh, Mesh* targetMesh,
                    const LandmarkManager& landmarkManager,
                    const ICPConfig& config);

    // Get progress (0-1) for UI
    float getProgress() const { return m_progress; }

    // Cancel ongoing alignment
    void cancel() { m_cancelled = true; }

private:
    float m_progress;
    bool m_cancelled;

    // Landmark-only alignment (Kabsch algorithm)
    Transform alignLandmarks(const std::vector<Vec3>& sourcePoints,
                             const std::vector<Vec3>& targetPoints,
                             bool allowScale);

    // Umeyama algorithm (with scale)
    Transform umeyamaAlignment(const std::vector<Vec3>& sourcePoints,
                               const std::vector<Vec3>& targetPoints);

    // Kabsch algorithm (rigid only)
    Transform kabschAlignment(const std::vector<Vec3>& sourcePoints,
                              const std::vector<Vec3>& targetPoints);

    // ICP iteration
    bool icpIteration(const Mesh* sourceMesh,
                      const std::vector<Vec3>& targetPoints,
                      Transform& currentTransform,
                      const ICPConfig& config,
                      float& residual);

    // Sample points from mesh
    std::vector<Vec3> sampleMeshPoints(const Mesh* mesh, int count);

    // Find nearest point on mesh
    Vec3 findNearestPoint(const Mesh* mesh, const Vec3& point);

    // Compute centroid
    Vec3 computeCentroid(const std::vector<Vec3>& points);

    // Compute RMS error
    float computeRMSError(const std::vector<Vec3>& sourcePoints,
                          const std::vector<Vec3>& targetPoints,
                          const Transform& transform);
};

} // namespace TopoShift
