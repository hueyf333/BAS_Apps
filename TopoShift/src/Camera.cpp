#include "Camera.h"
#include <algorithm>

namespace TopoShift {

Camera::Camera()
    : m_target(0, 0, 0), m_distance(5.0f), m_yaw(0.0f), m_pitch(0.3f),
      m_viewportWidth(800), m_viewportHeight(600),
      m_fov(XM_PIDIV4), m_nearPlane(0.1f), m_farPlane(1000.0f) {}

void Camera::setViewport(int width, int height) {
    m_viewportWidth = std::max(1, width);
    m_viewportHeight = std::max(1, height);
}

XMMATRIX Camera::getViewMatrix() const {
    Vec3 pos = getPosition();
    XMVECTOR eye = XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
    XMVECTOR target = XMVectorSet(m_target.x, m_target.y, m_target.z, 1.0f);
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);
    return XMMatrixLookAtLH(eye, target, up);
}

XMMATRIX Camera::getProjectionMatrix() const {
    float aspectRatio = static_cast<float>(m_viewportWidth) / static_cast<float>(m_viewportHeight);
    return XMMatrixPerspectiveFovLH(m_fov, aspectRatio, m_nearPlane, m_farPlane);
}

XMMATRIX Camera::getViewProjectionMatrix() const {
    return getViewMatrix() * getProjectionMatrix();
}

void Camera::orbit(float deltaYaw, float deltaPitch) {
    m_yaw += deltaYaw;
    m_pitch += deltaPitch;
    m_pitch = std::clamp(m_pitch, -XM_PIDIV2 + 0.01f, XM_PIDIV2 - 0.01f);
}

void Camera::pan(float deltaX, float deltaY) {
    // Get right and up vectors
    Vec3 pos = getPosition();
    Vec3 forward = (m_target - pos).normalized();
    Vec3 right = forward.cross(Vec3(0, 1, 0)).normalized();
    Vec3 up = right.cross(forward).normalized();

    m_target += right * deltaX + up * deltaY;
}

void Camera::zoom(float delta) {
    m_distance -= delta;
    m_distance = std::max(0.1f, m_distance);
}

Ray Camera::getRay(int screenX, int screenY) const {
    // Convert screen coords to NDC
    float ndcX = (2.0f * screenX / m_viewportWidth) - 1.0f;
    float ndcY = 1.0f - (2.0f * screenY / m_viewportHeight);

    // Unproject to world space
    XMMATRIX viewProj = getViewProjectionMatrix();
    XMMATRIX invViewProj = XMMatrixInverse(nullptr, viewProj);

    XMVECTOR nearPoint = XMVectorSet(ndcX, ndcY, 0.0f, 1.0f);
    XMVECTOR farPoint = XMVectorSet(ndcX, ndcY, 1.0f, 1.0f);

    nearPoint = XMVector3Transform(nearPoint, invViewProj);
    farPoint = XMVector3Transform(farPoint, invViewProj);

    // Perspective divide
    nearPoint = XMVectorDivide(nearPoint, XMVectorSplatW(nearPoint));
    farPoint = XMVectorDivide(farPoint, XMVectorSplatW(farPoint));

    XMFLOAT3 near, far;
    XMStoreFloat3(&near, nearPoint);
    XMStoreFloat3(&far, farPoint);

    Vec3 origin(near.x, near.y, near.z);
    Vec3 direction(far.x - near.x, far.y - near.y, far.z - near.z);

    return Ray(origin, direction.normalized());
}

void Camera::reset() {
    m_target = Vec3(0, 0, 0);
    m_distance = 5.0f;
    m_yaw = 0.0f;
    m_pitch = 0.3f;
}

void Camera::focus(const AABB& bounds) {
    m_target = bounds.center();
    Vec3 extent = bounds.extents();
    float maxExtent = std::max({extent.x, extent.y, extent.z});
    m_distance = maxExtent * 2.5f / std::tan(m_fov * 0.5f);
}

Vec3 Camera::getPosition() const {
    float x = m_distance * std::cos(m_pitch) * std::sin(m_yaw);
    float y = m_distance * std::sin(m_pitch);
    float z = m_distance * std::cos(m_pitch) * std::cos(m_yaw);
    return m_target + Vec3(x, y, z);
}

} // namespace TopoShift
