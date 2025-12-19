#pragma once

#include "audio/AudioBuffer.h"

#include <string>

namespace audio {

class Clip {
public:
    Clip(std::string name, double startTime, double length, AudioBuffer buffer);

    const std::string &name() const;
    double startTime() const;
    double length() const;
    double endTime() const;
    const AudioBuffer &buffer() const;

private:
    std::string m_name;
    double m_startTime{0.0};
    double m_length{0.0};
    AudioBuffer m_buffer;
};

} // namespace audio
