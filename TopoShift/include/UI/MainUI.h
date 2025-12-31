#pragma once

#include <string>
#include <vector>

namespace TopoShift {

class Application;

struct LogEntry {
    std::string message;
    float timestamp;
    
    LogEntry(const std::string& msg, float time) : message(msg), timestamp(time) {}
};

class MainUI {
public:
    MainUI();
    ~MainUI() = default;

    void render(Application& app);

    // Add log entry
    void addLog(const std::string& message);

    // Clear log
    void clearLog();

    // Panel visibility
    void toggleLandmarksPanel();
    void toggleAlignmentPanel();
    void toggleLogPanel();

private:
    bool m_showLogPanel;
    std::vector<LogEntry> m_logEntries;
    float m_currentTime;

    void renderMenuBar(Application& app);
    void renderLogPanel();
    void renderStatusBar(Application& app);
};

// Global UI instance (singleton)
MainUI& getMainUI();

} // namespace TopoShift
