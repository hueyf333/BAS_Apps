#pragma once

#include "Math.h"

namespace TopoShift {

class Camera {
public:
    Camera();
    ~Camera() = default;

    // Set viewport size
    void setViewport(int width, int height);

    // Get view and projection matrices
    XMMATRIX getViewMatrix() const;
    XMMATRIX getProjectionMatrix() const;
    XMMATRIX getViewProjectionMatrix() const;

    // Camera controls
    void orbit(float deltaYaw, float deltaPitch);
    void pan(float deltaX, float deltaY);
    void zoom(float delta);

    // Get ray from screen position (for picking)
    Ray getRay(int screenX, int screenY) const;

    // Reset camera to default position
    void reset();

    // Focus on bounding box
    void focus(const AABB& bounds);

    // Get camera position
    Vec3 getPosition() const;

private:
    Vec3 m_target;       // Look-at target
    float m_distance;    // Distance from target
    float m_yaw;         // Horizontal angle (radians)
    float m_pitch;       // Vertical angle (radians)
    
    int m_viewportWidth;
    int m_viewportHeight;
    float m_fov;         // Field of view (radians)
    float m_nearPlane;
    float m_farPlane;
};

} // namespace TopoShift
