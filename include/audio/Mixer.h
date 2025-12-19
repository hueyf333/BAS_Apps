#pragma once

#include "audio/Track.h"

#include <vector>

namespace audio {

class Mixer {
public:
    void addTrack(Track track);
    std::vector<Track> &tracks();
    const std::vector<Track> &tracks() const;

    void setMasterGainDb(float gainDb);
    float masterGainDb() const;

    AudioBuffer mix(std::size_t frames, double time);

private:
    std::vector<Track> m_tracks;
    float m_masterGainDb{0.0f};
};

} // namespace audio
