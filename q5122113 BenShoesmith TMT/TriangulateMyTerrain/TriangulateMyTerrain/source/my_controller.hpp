#pragma once

#include "my_view.hpp"

#include <tygra/WindowControlDelegate.hpp>
#include <scene/context.hpp>

class MyController : public tygra::WindowControlDelegate
{
public:

    MyController();

private:

    void windowControlWillStart(tygra::Window * window) override;

    void windowControlDidStop(tygra::Window * window) override;

    void windowControlViewWillRender(tygra::Window * window) override;

    void windowControlMouseMoved(tygra::Window * window,
                                 int x,
                                 int y) override;

    void windowControlMouseButtonChanged(tygra::Window * window,
                                         int button_index,
                                         bool down) override;

    void windowControlMouseWheelMoved(tygra::Window * window,
                                      int position) override;

    void windowControlKeyboardChanged(tygra::Window * window,
                                      int key_index,
                                      bool down) override;

    void windowControlGamepadAxisMoved(tygra::Window * window,
                                       int gamepad_index,
                                       int axis_index,
                                       float pos) override;

    void windowControlGamepadButtonChanged(tygra::Window * window,
                                           int gamepad_index,
                                           int button_index,
                                           bool down) override;

private:

    void updateCameraTranslation();

    std::unique_ptr<MyView> view_;
    std::unique_ptr<scene::Context> scene_;

    float camera_speed_{ 100.f };
    bool camera_turn_mode_{ false };
    float camera_move_speed_[4]{ 0.f, 0.f, 0.f, 0.f };
    float camera_rotate_speed_[2]{ 0.f, 0.f };

};
