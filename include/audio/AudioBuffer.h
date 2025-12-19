#pragma once

#include <cstddef>
#include <vector>

namespace audio {

class AudioBuffer {
public:
    AudioBuffer(std::size_t channels = 2, std::size_t frames = 0);

    void resize(std::size_t channels, std::size_t frames);
    void clear();

    std::size_t channels() const;
    std::size_t frames() const;

    float *data(std::size_t channel);
    const float *data(std::size_t channel) const;

    void addFrom(const AudioBuffer &other, float gain = 1.0f);

private:
    std::vector<float> m_data;
    std::size_t m_channels{0};
    std::size_t m_frames{0};
};

} // namespace audio
