#include "audio/Mixer.h"

#include <cmath>

namespace audio {

void Mixer::addTrack(Track track) {
    m_tracks.push_back(std::move(track));
}

std::vector<Track> &Mixer::tracks() {
    return m_tracks;
}

const std::vector<Track> &Mixer::tracks() const {
    return m_tracks;
}

void Mixer::setMasterGainDb(float gainDb) {
    m_masterGainDb = gainDb;
}

float Mixer::masterGainDb() const {
    return m_masterGainDb;
}

AudioBuffer Mixer::mix(std::size_t frames, double time) {
    AudioBuffer master(2, frames);
    bool anySolo = false;
    for (const auto &track : m_tracks) {
        if (track.isSolo()) {
            anySolo = true;
            break;
        }
    }

    for (auto &track : m_tracks) {
        track.prepareBuffer(frames);
        if (track.isMuted()) {
            continue;
        }
        if (anySolo && !track.isSolo()) {
            continue;
        }
        float automationGain = 0.0f;
        for (const auto &lane : track.automationLanes()) {
            if (lane.parameterName() == "gain") {
                automationGain = lane.valueAt(time);
            }
        }
        const float gainLinear = std::pow(10.0f, (track.gainDb() + automationGain) / 20.0f);
        master.addFrom(track.buffer(), gainLinear);
    }

    const float masterGainLinear = std::pow(10.0f, m_masterGainDb / 20.0f);
    for (std::size_t ch = 0; ch < master.channels(); ++ch) {
        float *samples = master.data(ch);
        for (std::size_t i = 0; i < master.frames(); ++i) {
            samples[i] *= masterGainLinear;
        }
    }

    return master;
}

} // namespace audio
