#include "Application.h"
#include "UI/MainUI.h"
#include <chrono>

namespace TopoShift {

Application::Application()
    : m_mouseX(0), m_mouseY(0), m_ctrlPressed(false), m_altPressed(false),
      m_isDragging(false), m_isProcessing(false), m_progress(0.0f) {
    m_mouseButtons[0] = m_mouseButtons[1] = m_mouseButtons[2] = false;
}

Application::~Application() {
    shutdown();
}

bool Application::initialize(HWND hwnd, int width, int height) {
    if (!m_renderer.initialize(hwnd, width, height)) {
        return false;
    }

    m_camera.setViewport(width, height);
    m_camera.reset();

    return true;
}

void Application::shutdown() {
    if (m_workerThread && m_workerThread->joinable()) {
        m_workerThread->join();
    }
    m_renderer.shutdown();
}

void Application::update(float deltaTime) {
    // Update landmark world positions if meshes have moved
    if (m_sourceMesh || m_targetMesh) {
        m_landmarkManager.updateWorldPositions();
    }
}

void Application::render() {
    m_renderer.beginFrame();

    // Render grid
    m_renderer.renderGrid(m_camera, 10.0f, 20);

    // Render meshes
    if (m_sourceMesh) {
        XMFLOAT4 sourceColor(0.2f, 0.7f, 0.3f, 1.0f);
        m_renderer.renderMesh(m_sourceMesh.get(), m_camera, sourceColor, false);
    }

    if (m_targetMesh) {
        XMFLOAT4 targetColor(0.7f, 0.3f, 0.2f, 1.0f);
        m_renderer.renderMesh(m_targetMesh.get(), m_camera, targetColor, true);
    }

    // Render landmarks
    m_renderer.renderLandmarks(m_landmarkManager, m_camera);
    m_renderer.renderLandmarkConnections(m_landmarkManager, m_camera);

    m_renderer.endFrame();
}

void Application::handleMouseMove(int x, int y) {
    int dx = x - m_mouseX;
    int dy = y - m_mouseY;

    if (m_mouseButtons[0] && !m_ctrlPressed && !m_altPressed) {
        // Left drag - orbit camera
        m_camera.orbit(dx * 0.01f, dy * 0.01f);
    } else if (m_mouseButtons[1] || (m_mouseButtons[0] && m_altPressed && !m_ctrlPressed)) {
        // Middle drag or Alt+Left - pan camera
        m_camera.pan(-dx * 0.01f, dy * 0.01f);
    }

    m_mouseX = x;
    m_mouseY = y;
}

void Application::handleMouseButton(int button, bool down, int x, int y) {
    m_mouseButtons[button] = down;

    if (!down && button == 0) {
        // Mouse up - check for picking
        if (m_ctrlPressed || m_altPressed) {
            handlePicking(x, y);
        }
    }
}

void Application::handleMouseWheel(float delta) {
    m_camera.zoom(delta * 0.1f);
}

void Application::handleKeyboard(int key, bool down) {
    if (key == VK_CONTROL) {
        m_ctrlPressed = down;
    } else if (key == VK_MENU) { // Alt key
        m_altPressed = down;
    }
}

void Application::resize(int width, int height) {
    m_renderer.resize(width, height);
    m_camera.setViewport(width, height);
}

bool Application::loadSourceMesh(const std::string& filename) {
    m_sourceMesh = std::make_unique<Mesh>();
    if (!m_sourceMesh->loadFromOBJ(filename)) {
        m_sourceMesh.reset();
        return false;
    }

    m_landmarkManager.initialize(m_sourceMesh.get(), m_targetMesh.get());
    
    // Focus camera on mesh
    AABB bounds = m_sourceMesh->getBoundingBox();
    if (m_targetMesh) {
        bounds.expand(m_targetMesh->getBoundingBox());
    }
    m_camera.focus(bounds);

    getMainUI().addLog("Loaded source mesh: " + filename);
    return true;
}

bool Application::loadTargetMesh(const std::string& filename) {
    m_targetMesh = std::make_unique<Mesh>();
    if (!m_targetMesh->loadFromOBJ(filename)) {
        m_targetMesh.reset();
        return false;
    }

    m_landmarkManager.initialize(m_sourceMesh.get(), m_targetMesh.get());
    
    // Focus camera on both meshes
    AABB bounds;
    if (m_sourceMesh) {
        bounds.expand(m_sourceMesh->getBoundingBox());
    }
    bounds.expand(m_targetMesh->getBoundingBox());
    m_camera.focus(bounds);

    getMainUI().addLog("Loaded target mesh: " + filename);
    return true;
}

bool Application::exportResult(const std::string& filename) {
    if (!m_targetMesh) {
        return false;
    }

    bool success = m_targetMesh->saveToOBJ(filename);
    if (success) {
        getMainUI().addLog("Exported result: " + filename);
    }
    return success;
}

void Application::runICPAlignment() {
    if (m_isProcessing) return;
    if (!m_sourceMesh || !m_targetMesh) return;

    m_isProcessing = true;
    setStatus("Running ICP alignment...");

    // Run in worker thread
    m_workerThread = std::make_unique<std::thread>([this]() {
        icpWorker();
    });
}

void Application::runWrapSolve() {
    if (m_isProcessing) return;
    if (!m_sourceMesh || !m_targetMesh) return;

    m_isProcessing = true;
    setStatus("Running wrap solve...");

    // Run in worker thread
    m_workerThread = std::make_unique<std::thread>([this]() {
        wrapWorker();
    });
}

void Application::handlePicking(int x, int y) {
    Ray ray = m_camera.getRay(x, y);
    
    // Determine which mesh to pick
    int meshId = m_ctrlPressed ? 0 : 1; // Ctrl = source, Alt = target
    
    // Get current selected landmark type from UI (simplified)
    LandmarkType selectedType = LandmarkType::NoseTip; // TODO: Get from UI
    
    if (m_landmarkManager.setLandmarkFromPick(selectedType, meshId, ray)) {
        const char* meshName = meshId == 0 ? "Source" : "Target";
        getMainUI().addLog(std::string("Set ") + getLandmarkName(selectedType) + 
                          " on " + meshName + " mesh");
    }
}

void Application::setStatus(const std::string& message) {
    std::lock_guard<std::mutex> lock(m_statusMutex);
    m_statusMessage = message;
}

void Application::icpWorker() {
    auto startTime = std::chrono::high_resolution_clock::now();

    m_icpResult = m_icpAligner.align(m_sourceMesh.get(), m_targetMesh.get(),
                                      m_landmarkManager, m_icpConfig);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    char buffer[256];
    sprintf_s(buffer, "ICP alignment completed in %.2f seconds", duration.count() / 1000.0f);
    getMainUI().addLog(buffer);

    sprintf_s(buffer, "RMS Landmark Error: %.4f", m_icpResult.rmsLandmarkError);
    getMainUI().addLog(buffer);

    sprintf_s(buffer, "RMS ICP Residual: %.4f", m_icpResult.rmsICPResidual);
    getMainUI().addLog(buffer);

    m_isProcessing = false;
    setStatus("ICP alignment complete");
}

void Application::wrapWorker() {
    auto startTime = std::chrono::high_resolution_clock::now();

    WrapResult result = m_wrapSolver.solve(m_targetMesh.get(), m_sourceMesh.get(),
                                            m_landmarkManager, m_wrapConfig);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    char buffer[256];
    sprintf_s(buffer, "Wrap solve completed in %.2f seconds", duration.count() / 1000.0f);
    getMainUI().addLog(buffer);

    sprintf_s(buffer, "Final energy: %.4f", result.finalEnergy);
    getMainUI().addLog(buffer);

    m_isProcessing = false;
    setStatus("Wrap solve complete");
}

} // namespace TopoShift
