#include "audio/AudioEngine.h"

namespace audio {

AudioEngine::AudioEngine(Project &project) : m_project(project) {}

AudioBuffer AudioEngine::process(std::size_t frames) {
    recordArmedTracks(frames);
    const double time = m_project.transport().time();
    AudioBuffer mix = m_project.mixer().mix(frames, time);
    m_project.transport().advance(static_cast<double>(frames) / m_project.sampleRate());
    return mix;
}

void AudioEngine::recordArmedTracks(std::size_t frames) {
    if (m_project.transport().state() != TransportState::Recording) {
        return;
    }
    for (auto &track : m_project.mixer().tracks()) {
        if (!track.isArmed()) {
            continue;
        }
        track.prepareBuffer(frames);
        for (std::size_t ch = 0; ch < track.channels(); ++ch) {
            float *samples = track.buffer().data(ch);
            for (std::size_t i = 0; i < frames; ++i) {
                samples[i] = 0.0f;
            }
        }
    }
}

} // namespace audio
