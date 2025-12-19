#pragma once

#include <string>
#include <vector>

namespace audio {

struct AutomationPoint {
    double time;
    float value;
};

class AutomationLane {
public:
    AutomationLane(std::string parameterName, float defaultValue);

    const std::string &parameterName() const;
    float defaultValue() const;

    void addPoint(double time, float value);
    float valueAt(double time) const;

private:
    std::string m_parameterName;
    float m_defaultValue{0.0f};
    std::vector<AutomationPoint> m_points;
};

} // namespace audio
