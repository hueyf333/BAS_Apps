#include "audio/Project.h"

#include <utility>

namespace audio {

Project::Project(std::string name, double sampleRate, double tempo)
    : m_name(std::move(name)), m_sampleRate(sampleRate), m_tempo(tempo), m_transport(sampleRate) {}

const std::string &Project::name() const {
    return m_name;
}

double Project::sampleRate() const {
    return m_sampleRate;
}

double Project::tempo() const {
    return m_tempo;
}

Mixer &Project::mixer() {
    return m_mixer;
}

const Mixer &Project::mixer() const {
    return m_mixer;
}

Transport &Project::transport() {
    return m_transport;
}

const Transport &Project::transport() const {
    return m_transport;
}

} // namespace audio
