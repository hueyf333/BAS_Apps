#pragma once

#include "audio/Project.h"

#include <string>

namespace audio {

class StudioUI {
public:
    explicit StudioUI(Project &project);

    void render();
    void showWelcomeScreen();

private:
    Project &m_project;

    void renderTransport();
    void renderTrackView();
    void renderMixer();
    void renderAutomation();
    void renderMeters();

    void header(const std::string &title) const;
};

} // namespace audio
