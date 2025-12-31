#include "ICPAligner.h"
#include "Mesh.h"
#include <random>
#include <DirectXMath.h>

using namespace DirectX;

namespace TopoShift {

ICPAligner::ICPAligner() : m_progress(0.0f), m_cancelled(false) {}

ICPResult ICPAligner::align(const Mesh* sourceMesh, Mesh* targetMesh,
                             const LandmarkManager& landmarkManager,
                             const ICPConfig& config) {
    m_cancelled = false;
    m_progress = 0.0f;
    ICPResult result;

    // Get complete landmark pairs
    std::vector<LandmarkPair> pairs = landmarkManager.getCompletePairs();
    
    if (pairs.empty() && config.useLandmarks) {
        result.success = false;
        return result;
    }

    // Extract landmark points
    std::vector<Vec3> sourcePoints, targetPoints;
    if (config.useLandmarks) {
        for (const auto& pair : pairs) {
            sourcePoints.push_back(pair.sourceRef.worldPosition);
            targetPoints.push_back(pair.targetRef.worldPosition);
        }
    }

    m_progress = 0.2f;

    // Initial alignment from landmarks
    Transform currentTransform;
    if (config.useLandmarks && !sourcePoints.empty()) {
        currentTransform = alignLandmarks(sourcePoints, targetPoints, config.allowUniformScale);
        result.rmsLandmarkError = computeRMSError(sourcePoints, targetPoints, currentTransform);
    }

    m_progress = 0.4f;

    // ICP refinement
    if (config.useICPRefine && sourceMesh && targetMesh) {
        std::vector<Vec3> targetSamples = sampleMeshPoints(targetMesh, config.sampleCount);
        
        for (int iter = 0; iter < config.icpIterations && !m_cancelled; ++iter) {
            float residual = 0.0f;
            if (!icpIteration(sourceMesh, targetSamples, currentTransform, config, residual)) {
                break;
            }
            result.rmsICPResidual = residual;
            m_progress = 0.4f + 0.6f * (iter + 1) / config.icpIterations;
        }
    }

    m_progress = 1.0f;

    // Apply transform to target if requested
    if (config.applyToTarget && targetMesh) {
        targetMesh->applyTransform(currentTransform);
    }

    result.transform = currentTransform;
    result.success = !m_cancelled;
    return result;
}

Transform ICPAligner::alignLandmarks(const std::vector<Vec3>& sourcePoints,
                                      const std::vector<Vec3>& targetPoints,
                                      bool allowScale) {
    if (allowScale) {
        return umeyamaAlignment(sourcePoints, targetPoints);
    } else {
        return kabschAlignment(sourcePoints, targetPoints);
    }
}

Transform ICPAligner::kabschAlignment(const std::vector<Vec3>& sourcePoints,
                                       const std::vector<Vec3>& targetPoints) {
    Transform result;
    
    if (sourcePoints.size() != targetPoints.size() || sourcePoints.empty()) {
        return result;
    }

    // Compute centroids
    Vec3 sourceCentroid = computeCentroid(sourcePoints);
    Vec3 targetCentroid = computeCentroid(targetPoints);

    // Center point sets
    std::vector<Vec3> sourceCentered, targetCentered;
    for (size_t i = 0; i < sourcePoints.size(); ++i) {
        sourceCentered.push_back(sourcePoints[i] - sourceCentroid);
        targetCentered.push_back(targetPoints[i] - targetCentroid);
    }

    // Compute covariance matrix H = sum(target * source^T)
    XMFLOAT3X3 H = {0};
    for (size_t i = 0; i < sourceCentered.size(); ++i) {
        const Vec3& s = sourceCentered[i];
        const Vec3& t = targetCentered[i];
        H._11 += t.x * s.x; H._12 += t.x * s.y; H._13 += t.x * s.z;
        H._21 += t.y * s.x; H._22 += t.y * s.y; H._23 += t.y * s.z;
        H._31 += t.z * s.x; H._32 += t.z * s.y; H._33 += t.z * s.z;
    }

    // SVD to find rotation (simplified - using identity for now)
    // In a complete implementation, perform SVD on H
    result.rotation = XMFLOAT4(0, 0, 0, 1); // Identity quaternion
    
    // Translation = targetCentroid - R * sourceCentroid
    result.translation = targetCentroid.toXMFLOAT3();
    result.scale = 1.0f;

    return result;
}

Transform ICPAligner::umeyamaAlignment(const std::vector<Vec3>& sourcePoints,
                                        const std::vector<Vec3>& targetPoints) {
    // Similar to Kabsch but computes scale
    // Simplified implementation
    return kabschAlignment(sourcePoints, targetPoints);
}

bool ICPAligner::icpIteration(const Mesh* sourceMesh,
                               const std::vector<Vec3>& targetPoints,
                               Transform& currentTransform,
                               const ICPConfig& config,
                               float& residual) {
    std::vector<Vec3> correspondences;
    correspondences.reserve(targetPoints.size());

    // Find correspondences
    int validCount = 0;
    residual = 0.0f;
    
    for (const Vec3& tp : targetPoints) {
        Vec3 transformed = currentTransform.transformPoint(tp);
        Vec3 nearest = findNearestPoint(sourceMesh, transformed);
        float dist = (nearest - transformed).length();
        
        if (dist < config.distanceThreshold) {
            correspondences.push_back(nearest);
            residual += dist * dist;
            validCount++;
        } else {
            correspondences.push_back(transformed); // Keep point as-is
        }
    }

    if (validCount < 3) return false; // Too few correspondences
    
    residual = std::sqrt(residual / validCount);

    // Update transform using correspondences
    Transform delta = kabschAlignment(targetPoints, correspondences);
    
    // Compose transforms
    XMMATRIX currentMat = currentTransform.toMatrix();
    XMMATRIX deltaMat = delta.toMatrix();
    currentTransform = Transform::fromMatrix(deltaMat * currentMat);

    return true;
}

std::vector<Vec3> ICPAligner::sampleMeshPoints(const Mesh* mesh, int count) {
    std::vector<Vec3> samples;
    const auto& vertices = mesh->getVertices();
    
    if (vertices.empty()) return samples;
    
    std::mt19937 rng(42);
    std::uniform_int_distribution<size_t> dist(0, vertices.size() - 1);
    
    for (int i = 0; i < count; ++i) {
        samples.push_back(vertices[dist(rng)]);
    }
    
    return samples;
}

Vec3 ICPAligner::findNearestPoint(const Mesh* mesh, const Vec3& point) {
    int idx = mesh->findNearestVertex(point);
    if (idx >= 0) {
        return mesh->getVertexPosition(idx);
    }
    return point;
}

Vec3 ICPAligner::computeCentroid(const std::vector<Vec3>& points) {
    Vec3 centroid(0, 0, 0);
    for (const auto& p : points) {
        centroid += p;
    }
    if (!points.empty()) {
        centroid /= static_cast<float>(points.size());
    }
    return centroid;
}

float ICPAligner::computeRMSError(const std::vector<Vec3>& sourcePoints,
                                   const std::vector<Vec3>& targetPoints,
                                   const Transform& transform) {
    if (sourcePoints.size() != targetPoints.size() || sourcePoints.empty()) {
        return 0.0f;
    }

    float sumSq = 0.0f;
    for (size_t i = 0; i < sourcePoints.size(); ++i) {
        Vec3 transformed = transform.transformPoint(targetPoints[i]);
        sumSq += (sourcePoints[i] - transformed).lengthSquared();
    }

    return std::sqrt(sumSq / sourcePoints.size());
}

} // namespace TopoShift
