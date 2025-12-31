#include "WrapSolver.h"
#include "Mesh.h"
#include <algorithm>
#include <cmath>

namespace TopoShift {

WrapSolver::WrapSolver() : m_progress(0.0f), m_cancelled(false) {}

WrapResult WrapSolver::solve(Mesh* targetMesh, const Mesh* sourceMesh,
                              const LandmarkManager& landmarkManager,
                              const WrapConfig& config) {
    m_cancelled = false;
    m_progress = 0.0f;
    WrapResult result;

    if (!targetMesh || !sourceMesh) {
        result.success = false;
        return result;
    }

    // Compute landmark weights for each vertex
    std::vector<float> weights = computeLandmarkWeights(targetMesh, landmarkManager, config);

    m_progress = 0.1f;

    // Iterative solving
    float energy = FLT_MAX;
    for (int iter = 0; iter < config.iterations && !m_cancelled; ++iter) {
        energy = solveIteration(targetMesh, sourceMesh, landmarkManager, weights, config);
        m_progress = 0.1f + 0.8f * (iter + 1) / config.iterations;
    }

    // Shrinkwrap refinement
    if (config.useShrinkwrap && !m_cancelled) {
        applyShrinkwrap(targetMesh, sourceMesh);
    }

    m_progress = 1.0f;

    result.success = !m_cancelled;
    result.finalEnergy = energy;
    result.iterations = config.iterations;
    return result;
}

std::vector<float> WrapSolver::computeLandmarkWeights(const Mesh* targetMesh,
                                                       const LandmarkManager& landmarkManager,
                                                       const WrapConfig& config) {
    size_t vertexCount = targetMesh->getVertexCount();
    std::vector<float> weights(vertexCount, 0.0f);

    // Get complete landmark pairs
    std::vector<LandmarkPair> pairs = landmarkManager.getCompletePairs();
    
    for (const auto& pair : pairs) {
        if (!pair.targetRef.isSet) continue;

        // Get landmark position
        Vec3 landmarkPos = pair.targetRef.worldPosition;

        // Apply weight to nearby vertices based on falloff
        const auto& vertices = targetMesh->getVertices();
        for (size_t i = 0; i < vertices.size(); ++i) {
            float dist = (vertices[i] - landmarkPos).length();
            float weight = computeFalloff(dist, config.landmarkFalloffRadius);
            weights[i] = std::max(weights[i], weight);
        }
    }

    // Scale weights by landmark weight parameter
    for (auto& w : weights) {
        w *= config.landmarkWeight;
    }

    return weights;
}

float WrapSolver::computeFalloff(float distance, float radius) {
    if (distance >= radius) return 0.0f;
    
    // Smooth falloff: (1 - (d/r)^2)^2
    float t = distance / radius;
    float s = 1.0f - t * t;
    return s * s;
}

float WrapSolver::solveIteration(Mesh* targetMesh, const Mesh* sourceMesh,
                                  const LandmarkManager& landmarkManager,
                                  const std::vector<float>& weights,
                                  const WrapConfig& config) {
    auto& vertices = targetMesh->getVertices();
    std::vector<Vec3> newPositions = vertices;

    float totalEnergy = 0.0f;

    // For each vertex, blend between current position and projection
    for (size_t i = 0; i < vertices.size(); ++i) {
        Vec3 projected = projectToMesh(vertices[i], sourceMesh);
        
        float w = weights[i];
        if (config.pinnedLandmarks && w > 0.5f) {
            w = 100.0f; // Very high weight for pinned vertices
        }

        // Blend based on weight
        float alpha = std::min(1.0f, w * 0.01f); // Scale weight to [0,1]
        newPositions[i] = vertices[i] * (1.0f - alpha) + projected * alpha;

        float energy = (newPositions[i] - projected).lengthSquared();
        totalEnergy += energy;
    }

    // Update positions
    vertices = newPositions;
    targetMesh->computeNormals();

    return totalEnergy / vertices.size();
}

void WrapSolver::applyShrinkwrap(Mesh* targetMesh, const Mesh* sourceMesh) {
    auto& vertices = targetMesh->getVertices();
    
    for (auto& v : vertices) {
        v = projectToMesh(v, sourceMesh);
    }
    
    targetMesh->computeNormals();
}

Vec3 WrapSolver::projectToMesh(const Vec3& vertex, const Mesh* sourceMesh) {
    // Simple nearest vertex projection
    // In a complete implementation, use proper surface projection
    int nearestIdx = sourceMesh->findNearestVertex(vertex);
    if (nearestIdx >= 0) {
        return sourceMesh->getVertexPosition(nearestIdx);
    }
    return vertex;
}

} // namespace TopoShift
