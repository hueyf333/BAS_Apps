#include "audio/Transport.h"

namespace audio {

Transport::Transport(double sampleRate) : m_sampleRate(sampleRate) {}

void Transport::play() {
    m_state = TransportState::Playing;
}

void Transport::stop() {
    m_state = TransportState::Stopped;
}

void Transport::record() {
    m_state = TransportState::Recording;
}

void Transport::seek(double timeInSeconds) {
    m_time = timeInSeconds;
}

void Transport::advance(double seconds) {
    if (m_state != TransportState::Stopped) {
        m_time += seconds;
    }
}

double Transport::time() const {
    return m_time;
}

double Transport::sampleRate() const {
    return m_sampleRate;
}

TransportState Transport::state() const {
    return m_state;
}

} // namespace audio
