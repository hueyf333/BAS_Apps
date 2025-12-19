#pragma once

namespace audio {

enum class TransportState {
    Stopped,
    Playing,
    Recording
};

class Transport {
public:
    explicit Transport(double sampleRate);

    void play();
    void stop();
    void record();

    void seek(double timeInSeconds);
    void advance(double seconds);

    double time() const;
    double sampleRate() const;
    TransportState state() const;

private:
    double m_sampleRate;
    double m_time{0.0};
    TransportState m_state{TransportState::Stopped};
};

} // namespace audio
