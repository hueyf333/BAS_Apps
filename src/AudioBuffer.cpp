#include "audio/AudioBuffer.h"

#include <algorithm>
#include <stdexcept>

namespace audio {

AudioBuffer::AudioBuffer(std::size_t channels, std::size_t frames) {
    resize(channels, frames);
}

void AudioBuffer::resize(std::size_t channels, std::size_t frames) {
    m_channels = channels;
    m_frames = frames;
    m_data.assign(channels * frames, 0.0f);
}

void AudioBuffer::clear() {
    std::fill(m_data.begin(), m_data.end(), 0.0f);
}

std::size_t AudioBuffer::channels() const {
    return m_channels;
}

std::size_t AudioBuffer::frames() const {
    return m_frames;
}

float *AudioBuffer::data(std::size_t channel) {
    if (channel >= m_channels) {
        throw std::out_of_range("channel out of range");
    }
    return m_data.data() + channel * m_frames;
}

const float *AudioBuffer::data(std::size_t channel) const {
    if (channel >= m_channels) {
        throw std::out_of_range("channel out of range");
    }
    return m_data.data() + channel * m_frames;
}

void AudioBuffer::addFrom(const AudioBuffer &other, float gain) {
    if (other.channels() != m_channels || other.frames() != m_frames) {
        throw std::invalid_argument("buffer mismatch");
    }
    for (std::size_t ch = 0; ch < m_channels; ++ch) {
        const float *src = other.data(ch);
        float *dst = data(ch);
        for (std::size_t i = 0; i < m_frames; ++i) {
            dst[i] += src[i] * gain;
        }
    }
}

} // namespace audio
