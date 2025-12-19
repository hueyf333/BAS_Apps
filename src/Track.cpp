#include "audio/Track.h"

#include <algorithm>

namespace audio {

Track::Track(int id, std::string name, std::size_t channels)
    : m_id(id), m_name(std::move(name)), m_channels(channels), m_buffer(channels, 0) {}

int Track::id() const {
    return m_id;
}

const std::string &Track::name() const {
    return m_name;
}

std::size_t Track::channels() const {
    return m_channels;
}

void Track::setArmed(bool armed) {
    m_armed = armed;
}

void Track::setMuted(bool muted) {
    m_muted = muted;
}

void Track::setSolo(bool solo) {
    m_solo = solo;
}

void Track::setGainDb(float gainDb) {
    m_gainDb = gainDb;
}

void Track::setPan(float pan) {
    m_pan = std::clamp(pan, -1.0f, 1.0f);
}

bool Track::isArmed() const {
    return m_armed;
}

bool Track::isMuted() const {
    return m_muted;
}

bool Track::isSolo() const {
    return m_solo;
}

float Track::gainDb() const {
    return m_gainDb;
}

float Track::pan() const {
    return m_pan;
}

void Track::addClip(Clip clip) {
    m_clips.push_back(std::move(clip));
}

const std::vector<Clip> &Track::clips() const {
    return m_clips;
}

void Track::prepareBuffer(std::size_t frames) {
    if (m_buffer.frames() != frames || m_buffer.channels() != m_channels) {
        m_buffer.resize(m_channels, frames);
    }
    m_buffer.clear();
}

const AudioBuffer &Track::buffer() const {
    return m_buffer;
}

AudioBuffer &Track::buffer() {
    return m_buffer;
}

AutomationLane &Track::automation(const std::string &parameter) {
    auto it = std::find_if(m_automationLanes.begin(), m_automationLanes.end(),
                           [&](const AutomationLane &lane) { return lane.parameterName() == parameter; });
    if (it != m_automationLanes.end()) {
        return *it;
    }
    m_automationLanes.emplace_back(parameter, 0.0f);
    return m_automationLanes.back();
}

const std::vector<AutomationLane> &Track::automationLanes() const {
    return m_automationLanes;
}

} // namespace audio
