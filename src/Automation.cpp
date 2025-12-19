#include "audio/Automation.h"

#include <algorithm>

namespace audio {

AutomationLane::AutomationLane(std::string parameterName, float defaultValue)
    : m_parameterName(std::move(parameterName)), m_defaultValue(defaultValue) {}

const std::string &AutomationLane::parameterName() const {
    return m_parameterName;
}

float AutomationLane::defaultValue() const {
    return m_defaultValue;
}

void AutomationLane::addPoint(double time, float value) {
    m_points.push_back({time, value});
    std::sort(m_points.begin(), m_points.end(), [](const AutomationPoint &a, const AutomationPoint &b) {
        return a.time < b.time;
    });
}

float AutomationLane::valueAt(double time) const {
    if (m_points.empty()) {
        return m_defaultValue;
    }
    if (time <= m_points.front().time) {
        return m_points.front().value;
    }
    if (time >= m_points.back().time) {
        return m_points.back().value;
    }
    for (std::size_t i = 1; i < m_points.size(); ++i) {
        if (time <= m_points[i].time) {
            const auto &prev = m_points[i - 1];
            const auto &next = m_points[i];
            const double t = (time - prev.time) / (next.time - prev.time);
            return static_cast<float>(prev.value + (next.value - prev.value) * t);
        }
    }
    return m_defaultValue;
}

} // namespace audio
