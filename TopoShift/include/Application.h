#pragma once

#include "Renderer.h"
#include "Camera.h"
#include "Mesh.h"
#include "LandmarkManager.h"
#include "ICPAligner.h"
#include "WrapSolver.h"
#include <Windows.h>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

namespace TopoShift {

class Application {
public:
    Application();
    ~Application();

    // Initialize application
    bool initialize(HWND hwnd, int width, int height);

    // Shutdown
    void shutdown();

    // Update and render
    void update(float deltaTime);
    void render();

    // Handle input
    void handleMouseMove(int x, int y);
    void handleMouseButton(int button, bool down, int x, int y);
    void handleMouseWheel(float delta);
    void handleKeyboard(int key, bool down);

    // Handle resize
    void resize(int width, int height);

    // Load meshes
    bool loadSourceMesh(const std::string& filename);
    bool loadTargetMesh(const std::string& filename);

    // Export result
    bool exportResult(const std::string& filename);

    // Run ICP alignment
    void runICPAlignment();

    // Run wrap solve
    void runWrapSolve();

    // Get status
    bool isProcessing() const { return m_isProcessing; }
    const std::string& getStatusMessage() const { return m_statusMessage; }

    // Get progress
    float getProgress() const { return m_progress; }

    // Access components
    Renderer& getRenderer() { return m_renderer; }
    Camera& getCamera() { return m_camera; }
    LandmarkManager& getLandmarkManager() { return m_landmarkManager; }
    ICPConfig& getICPConfig() { return m_icpConfig; }
    WrapConfig& getWrapConfig() { return m_wrapConfig; }
    
    Mesh* getSourceMesh() { return m_sourceMesh.get(); }
    Mesh* getTargetMesh() { return m_targetMesh.get(); }
    const Mesh* getSourceMesh() const { return m_sourceMesh.get(); }
    const Mesh* getTargetMesh() const { return m_targetMesh.get(); }

    ICPResult& getICPResult() { return m_icpResult; }
    const ICPResult& getICPResult() const { return m_icpResult; }

private:
    Renderer m_renderer;
    Camera m_camera;
    
    std::unique_ptr<Mesh> m_sourceMesh;
    std::unique_ptr<Mesh> m_targetMesh;
    
    LandmarkManager m_landmarkManager;
    ICPAligner m_icpAligner;
    WrapSolver m_wrapSolver;

    ICPConfig m_icpConfig;
    WrapConfig m_wrapConfig;
    ICPResult m_icpResult;

    // UI state
    int m_mouseX, m_mouseY;
    bool m_mouseButtons[3]; // Left, Middle, Right
    bool m_ctrlPressed;
    bool m_altPressed;
    bool m_isDragging;

    // Processing state
    std::atomic<bool> m_isProcessing;
    std::atomic<float> m_progress;
    std::string m_statusMessage;
    std::mutex m_statusMutex;

    // Worker thread
    std::unique_ptr<std::thread> m_workerThread;

    // Handle picking
    void handlePicking(int x, int y);

    // Update status
    void setStatus(const std::string& message);

    // Worker thread functions
    void icpWorker();
    void wrapWorker();
};

} // namespace TopoShift
