#include "UI/MainUI.h"
#include "Application.h"
// Note: ImGui headers would be included here
// #include "imgui.h"

namespace TopoShift {

static MainUI* g_mainUI = nullptr;

MainUI& getMainUI() {
    if (!g_mainUI) {
        g_mainUI = new MainUI();
    }
    return *g_mainUI;
}

MainUI::MainUI() : m_showLogPanel(true), m_currentTime(0.0f) {}

void MainUI::render(Application& app) {
    m_currentTime += 0.016f; // Approximate frame time

    renderMenuBar(app);
    renderLogPanel();
    renderStatusBar(app);
}

void MainUI::renderMenuBar(Application& app) {
    // Note: This is a stub showing the intended ImGui structure

    /*
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load Source Mesh...")) {
                // TODO: File dialog
                app.loadSourceMesh("source.obj");
            }
            if (ImGui::MenuItem("Load Target Mesh...")) {
                // TODO: File dialog
                app.loadTargetMesh("target.obj");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Export Result...")) {
                // TODO: File dialog
                app.exportResult("result.obj");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                PostQuitMessage(0);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Landmarks", "F1", nullptr)) {
                toggleLandmarksPanel();
            }
            if (ImGui::MenuItem("Alignment", "F2", nullptr)) {
                toggleAlignmentPanel();
            }
            if (ImGui::MenuItem("Log", "F3", nullptr)) {
                toggleLogPanel();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Process")) {
            if (ImGui::MenuItem("Run ICP Alignment")) {
                app.runICPAlignment();
            }
            if (ImGui::MenuItem("Run Wrap Solve")) {
                app.runWrapSolve();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
    */
}

void MainUI::renderLogPanel() {
    if (!m_showLogPanel) return;

    // Note: This is a stub showing the intended ImGui structure

    /*
    ImGui::Begin("Log", &m_showLogPanel);

    if (ImGui::Button("Clear")) {
        clearLog();
    }

    ImGui::Separator();

    ImGui::BeginChild("LogScroll", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& entry : m_logEntries) {
        ImGui::Text("[%.2f] %s", entry.timestamp, entry.message.c_str());
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();

    ImGui::End();
    */
}

void MainUI::renderStatusBar(Application& app) {
    // Note: This is a stub showing the intended ImGui structure

    /*
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - 25));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 25));
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoSavedSettings;

    ImGui::Begin("StatusBar", nullptr, flags);
    
    if (app.isProcessing()) {
        ImGui::Text("Processing... %.0f%%", app.getProgress() * 100.0f);
    } else {
        ImGui::Text("%s", app.getStatusMessage().c_str());
    }

    ImGui::End();
    */
}

void MainUI::addLog(const std::string& message) {
    m_logEntries.emplace_back(message, m_currentTime);
}

void MainUI::clearLog() {
    m_logEntries.clear();
}

void MainUI::toggleLandmarksPanel() {
    // TODO: Toggle landmark panel visibility
}

void MainUI::toggleAlignmentPanel() {
    // TODO: Toggle alignment panel visibility
}

void MainUI::toggleLogPanel() {
    m_showLogPanel = !m_showLogPanel;
}

} // namespace TopoShift
