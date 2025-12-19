#pragma once

#include "audio/AudioBuffer.h"
#include "audio/Automation.h"
#include "audio/Clip.h"

#include <string>
#include <vector>

namespace audio {

class Track {
public:
    Track(int id, std::string name, std::size_t channels);

    int id() const;
    const std::string &name() const;
    std::size_t channels() const;

    void setArmed(bool armed);
    void setMuted(bool muted);
    void setSolo(bool solo);
    void setGainDb(float gainDb);
    void setPan(float pan);

    bool isArmed() const;
    bool isMuted() const;
    bool isSolo() const;
    float gainDb() const;
    float pan() const;

    void addClip(Clip clip);
    const std::vector<Clip> &clips() const;

    void prepareBuffer(std::size_t frames);
    const AudioBuffer &buffer() const;
    AudioBuffer &buffer();

    AutomationLane &automation(const std::string &parameter);
    const std::vector<AutomationLane> &automationLanes() const;

private:
    int m_id;
    std::string m_name;
    std::size_t m_channels;
    bool m_armed{false};
    bool m_muted{false};
    bool m_solo{false};
    float m_gainDb{0.0f};
    float m_pan{0.0f};
    AudioBuffer m_buffer;
    std::vector<Clip> m_clips;
    std::vector<AutomationLane> m_automationLanes;
};

} // namespace audio
