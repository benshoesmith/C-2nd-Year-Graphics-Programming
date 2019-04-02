#pragma once

#include "types.hpp"

namespace scene {

class Camera
{
  public:
    bool isStatic() const { return false; }

    Vector3 getPosition() const;

    Vector3 getDirection() const;

    float getVerticalFieldOfViewInDegrees() const;

    float getNearPlaneDistance() const;

    float getFarPlaneDistance() const;

    Vector3 getLinearVelocity() const;

    Vector2 getRotationalVelocity() const;

    void setPosition(Vector3 p);
    void setDirection(Vector3 d);
    void setVerticalFieldOfViewInDegrees(float d);
    void setNearPlaneDistance(float n);
    void setFarPlaneDistance(float f);
    void setLinearVelocity(Vector3 v);
    void setRotationalVelocity(Vector2 v);

  private:
    Vector3 position_{ 0, 0, 0 };
    Vector3 direction_{ 0, 0, -1 };
    float vertical_field_of_view_degrees_{ 60 };
    float near_plane_distance_{ 1 };
    float far_plane_distance_{ 10000 };
    Vector3 translation_speed_{ 0, 0, 0 };
    Vector2 rotation_speed_{ 0, 0 };
};

} // end namespace scene
