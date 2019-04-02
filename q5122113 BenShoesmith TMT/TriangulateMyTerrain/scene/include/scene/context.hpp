#pragma once

#include "types.hpp"
#include "camera.hpp"

#include <vector>
#include <chrono>
#include <memory>

namespace scene {

class FirstPersonMovement;

class Context
{
public:
    Context();
    ~Context();

    // Number of bi-cubic Bezier patches
    size_t getTerrainPatchCount() const;

    // Control points for bi-cubic Bezier patches
    const Vector3& getTerrainPatchPoint(size_t patch_index,
                                        size_t index_u,
                                        size_t index_v) const;

    std::string getTerrainDisplacementMapName() const;

    // X and -Z are on the ground, Y is up
    float getTerrainSizeX() const;
    float getTerrainSizeY() const;
    float getTerrainSizeZ() const;

    void update();

    float getTimeInSeconds() const;

    const Camera& getCamera() const;
    Camera& getCamera();

private:
    Vector3& getTerrainPatchPoint(size_t patch_index,
                                  size_t index_u,
                                  size_t index_v);

    std::chrono::system_clock::time_point start_time_;
    float time_seconds_{ 0.f };

    std::unique_ptr<FirstPersonMovement> camera_movement_;
    std::unique_ptr<Camera> camera_;

    std::vector<Vector3> _points;
};

} // end namespace scene
