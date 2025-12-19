#include "audio/Clip.h"

#include <utility>

namespace audio {

Clip::Clip(std::string name, double startTime, double length, AudioBuffer buffer)
    : m_name(std::move(name)), m_startTime(startTime), m_length(length), m_buffer(std::move(buffer)) {}

const std::string &Clip::name() const {
    return m_name;
}

double Clip::startTime() const {
    return m_startTime;
}

double Clip::length() const {
    return m_length;
}

double Clip::endTime() const {
    return m_startTime + m_length;
}

const AudioBuffer &Clip::buffer() const {
    return m_buffer;
}

} // namespace audio
