#pragma once

#include "audio/Project.h"

#include <cstddef>

namespace audio {

class AudioEngine {
public:
    explicit AudioEngine(Project &project);

    AudioBuffer process(std::size_t frames);
    void recordArmedTracks(std::size_t frames);

private:
    Project &m_project;
};

} // namespace audio
