#include "scene/context.hpp"
#include "first_person_movement.hpp"
#include <random>

using namespace scene;

Context::Context() : _points(32)
{
    start_time_ = std::chrono::system_clock::now();

    camera_movement_ = std::make_unique<FirstPersonMovement>();
    camera_movement_->init(Vector3(0, 16, 0), -0.785f, -0.25f);

    camera_ = std::make_unique<Camera>();
    camera_->setVerticalFieldOfViewInDegrees(50.f);
    camera_->setNearPlaneDistance(1);
    camera_->setFarPlaneDistance(12000);

    getTerrainPatchPoint(0, 0, 0) = Vector3(0, 0, 0);
    getTerrainPatchPoint(0, 0, 1) = Vector3(0, 0, -2731);
    getTerrainPatchPoint(0, 0, 2) = Vector3(0, 0, -5461);
    getTerrainPatchPoint(0, 0, 3) = Vector3(0, 538, -8192);
    getTerrainPatchPoint(0, 1, 0) = Vector3(1365, 0, 0);
    getTerrainPatchPoint(0, 1, 1) = Vector3(2156, 1333, -2731);
    getTerrainPatchPoint(0, 1, 2) = Vector3(2156, 2666, -5461);
    getTerrainPatchPoint(0, 1, 3) = Vector3(1365, 0, -8192);
    getTerrainPatchPoint(0, 2, 0) = Vector3(1051, -1611, 182);
    getTerrainPatchPoint(0, 2, 1) = Vector3(1051, -1241, -2574);
    getTerrainPatchPoint(0, 2, 2) = Vector3(1051, -871, -5329);
    getTerrainPatchPoint(0, 2, 3) = Vector3(1051, -501, -8085);
    getTerrainPatchPoint(0, 3, 0) = Vector3(4091, -556, 38);
    getTerrainPatchPoint(0, 3, 1) = Vector3(4091, -186, -2718);
    getTerrainPatchPoint(0, 3, 2) = Vector3(4091, 184, -5474);
    getTerrainPatchPoint(0, 3, 3) = Vector3(4091, 554, -8192);

    getTerrainPatchPoint(1, 0, 0) = Vector3(4091, -556, 38);
    getTerrainPatchPoint(1, 0, 1) = Vector3(4091, -186, -2718);
    getTerrainPatchPoint(1, 0, 2) = Vector3(4091, 184, -5474);
    getTerrainPatchPoint(1, 0, 3) = Vector3(4091, 554, -8192);
    getTerrainPatchPoint(1, 1, 0) = Vector3(7137, 1437, -106);
    getTerrainPatchPoint(1, 1, 1) = Vector3(7141, 867, -2862);
    getTerrainPatchPoint(1, 1, 2) = Vector3(7136, 1238, -5618);
    getTerrainPatchPoint(1, 1, 3) = Vector3(7131, 1609, -8192);
    getTerrainPatchPoint(1, 2, 0) = Vector3(6827, 939, 0);
    getTerrainPatchPoint(1, 2, 1) = Vector3(6827, 0, -2731);
    getTerrainPatchPoint(1, 2, 2) = Vector3(6827, 0, -5461);
    getTerrainPatchPoint(1, 2, 3) = Vector3(6827, 0, -8192);
    getTerrainPatchPoint(1, 3, 0) = Vector3(8192, 939, 0);
    getTerrainPatchPoint(1, 3, 1) = Vector3(8192, 939, -2731);
    getTerrainPatchPoint(1, 3, 2) = Vector3(8192, 0, -5461);
    getTerrainPatchPoint(1, 3, 3) = Vector3(8192, 0, -8192);

    update();
}

Context::~Context()
{
}

size_t Context::getTerrainPatchCount() const
{
    return _points.size() / 16;
}

const Vector3& Context::getTerrainPatchPoint(size_t patch_index,
                                             size_t index_u,
                                             size_t index_v) const
{
    return _points[16 * patch_index + 4 * index_v + index_u];
}

Vector3& Context::getTerrainPatchPoint(size_t patch_index,
                                       size_t index_u,
                                       size_t index_v)
{
    return _points[16 * patch_index + 4 * index_v + index_u];
}

std::string Context::getTerrainDisplacementMapName() const
{
    return "content:///terrain.png";
}

float Context::getTerrainSizeX() const
{
    return 8192;
}

float Context::getTerrainSizeY() const
{
    return 1000;
}

float Context::getTerrainSizeZ() const
{
    return 8192;
}

void Context::update()
{
    const auto clock_time = std::chrono::system_clock::now() - start_time_;
    const auto clock_millisecs
        = std::chrono::duration_cast<std::chrono::milliseconds>(clock_time);
    const float prev_time = time_seconds_;
    time_seconds_ = 0.001f * clock_millisecs.count();
    const float dt = time_seconds_ - prev_time;

    auto camera_translation_speed = getCamera().getLinearVelocity();
    auto camera_rotation_speed = getCamera().getRotationalVelocity();
    camera_movement_->moveForward(camera_translation_speed.z * dt);
    camera_movement_->moveRight(camera_translation_speed.x * dt);
    camera_movement_->spinHorizontal(camera_rotation_speed.x * dt);
    camera_movement_->spinVertical(camera_rotation_speed.y * dt);
    camera_->setPosition(camera_movement_->position());
    camera_->setDirection(camera_movement_->direction());
}

float Context::getTimeInSeconds() const
{
    return time_seconds_;
}

const Camera& Context::getCamera() const
{
    return *camera_;
}

Camera& Context::getCamera()
{
    return *camera_;
}
