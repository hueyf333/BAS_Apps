#pragma once

#include "audio/Mixer.h"
#include "audio/Transport.h"

#include <string>

namespace audio {

class Project {
public:
    Project(std::string name, double sampleRate, double tempo);

    const std::string &name() const;
    double sampleRate() const;
    double tempo() const;

    Mixer &mixer();
    const Mixer &mixer() const;

    Transport &transport();
    const Transport &transport() const;

private:
    std::string m_name;
    double m_sampleRate;
    double m_tempo;
    Mixer m_mixer;
    Transport m_transport;
};

} // namespace audio
