#include "audio/StudioUI.h"

#include <iomanip>
#include <iostream>

namespace audio {

StudioUI::StudioUI(Project &project) : m_project(project) {}

void StudioUI::render() {
    showWelcomeScreen();
    renderTransport();
    renderTrackView();
    renderMixer();
    renderAutomation();
    renderMeters();
}

void StudioUI::showWelcomeScreen() {
    header("BAS Studio - Professional Multitrack Environment");
    std::cout << "Project: " << m_project.name() << "\n";
    std::cout << "Sample Rate: " << m_project.sampleRate() << " Hz" << " | Tempo: " << m_project.tempo()
              << " BPM\n\n";
}

void StudioUI::renderTransport() {
    header("Transport");
    const auto &transport = m_project.transport();
    std::string state;
    switch (transport.state()) {
        case TransportState::Stopped:
            state = "Stopped";
            break;
        case TransportState::Playing:
            state = "Playing";
            break;
        case TransportState::Recording:
            state = "Recording";
            break;
    }
    std::cout << "State: " << state << " | Time: " << std::fixed << std::setprecision(2) << transport.time()
              << " s\n\n";
}

void StudioUI::renderTrackView() {
    header("Track View");
    for (const auto &track : m_project.mixer().tracks()) {
        std::cout << "Track " << track.id() << ": " << track.name() << " | Clips: " << track.clips().size()
                  << " | Armed: " << (track.isArmed() ? "Yes" : "No") << "\n";
    }
    std::cout << "\n";
}

void StudioUI::renderMixer() {
    header("Mixer");
    for (const auto &track : m_project.mixer().tracks()) {
        std::cout << track.name() << " | Gain: " << track.gainDb() << " dB | Pan: " << track.pan()
                  << " | Mute: " << (track.isMuted() ? "Yes" : "No") << " | Solo: "
                  << (track.isSolo() ? "Yes" : "No") << "\n";
    }
    std::cout << "Master Gain: " << m_project.mixer().masterGainDb() << " dB\n\n";
}

void StudioUI::renderAutomation() {
    header("Automation");
    for (const auto &track : m_project.mixer().tracks()) {
        std::cout << track.name() << " automation lanes: " << track.automationLanes().size() << "\n";
    }
    std::cout << "\n";
}

void StudioUI::renderMeters() {
    header("Meters");
    std::cout << "Master Output: -6.0 dBFS (simulated)\n";
    std::cout << "\n";
}

void StudioUI::header(const std::string &title) const {
    std::cout << "== " << title << " ==\n";
}

} // namespace audio
