#include "EventSystem.h"

#pragma once

#define INPUT Acidrain::InputProvider::getInstance()

namespace Acidrain {

    struct JoystickState {
        JoystickState() {
            for (int i = 0; i < 12; i++)
                buttons[i] = false;
        };

        int x_axis = 0;
        int y_axis = 0;
        bool buttons[12];
    };

    struct KeyboardState {
        KeyboardState() {
            for (int i = 0; i < 256; i++)
                keys[i] = false;
        };
        bool keys[256];
    };

    class InputProvider : public EventListener {
    public:

        static InputProvider& getInstance();

        ~InputProvider();

        bool isKeyDown(int keyCode) const;

        bool isKeyUp(int keyCode) const;

        bool isKeyJustPressed(int keyCode) const;

        bool isKeyJustReleased(int keyCode) const;

        bool isJoystickPressedLeft() const;

        bool isJoystickJustPressedLeft() const;

        bool isJoystickPressedRight() const;

        bool isJoystickJustPressedRight() const;

        bool isJoystickPressedUp() const;

        bool isJoystickJustPressedUp() const;

        bool isJoystickPressedDown() const;

        bool isJoystickJustPressedDown() const;

        void copyNewStateToOldState();

        virtual void onEvent(SDL_Event const& param) override;

    private:
        InputProvider();

        KeyboardState keyboardState, oldKeyboardState;
        JoystickState joystickState, oldJoystickState;
        std::vector<SDL_Joystick*> joysticks;
    };
}