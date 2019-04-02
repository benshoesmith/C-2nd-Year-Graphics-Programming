#include "scene/camera.hpp"

using namespace scene;

Vector3 Camera::getPosition() const
{
    return position_;
}

void Camera::setPosition(Vector3 p)
{
    position_ = p;
}

Vector3 Camera::getDirection() const
{
    return direction_;
}

void Camera::setDirection(Vector3 d)
{
    direction_ = d;
}

float Camera::getVerticalFieldOfViewInDegrees() const
{
    return vertical_field_of_view_degrees_;
}

void Camera::setVerticalFieldOfViewInDegrees(float d)
{
    vertical_field_of_view_degrees_ = d;
}

float Camera::getNearPlaneDistance() const
{
    return near_plane_distance_;
}

void Camera::setNearPlaneDistance(float n)
{
    near_plane_distance_ = n;
}

float Camera::getFarPlaneDistance() const
{
    return far_plane_distance_;
}

void Camera::setFarPlaneDistance(float f)
{
    far_plane_distance_ = f;
}

Vector3 Camera::getLinearVelocity() const
{
    return translation_speed_;
}

void Camera::setLinearVelocity(Vector3 v)
{
    translation_speed_ = v;
}

Vector2 Camera::getRotationalVelocity() const
{
    return rotation_speed_;
}

void Camera::setRotationalVelocity(Vector2 v)
{
    rotation_speed_ = v;
}
