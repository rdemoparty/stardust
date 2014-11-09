#include "InputProvider.h"
#include <iostream>

namespace Acidrain {

    InputProvider::InputProvider() {
        if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0)
            SDL_InitSubSystem(SDL_INIT_JOYSTICK);

        if (SDL_NumJoysticks() > 0) {
            for (int i = 0; i < SDL_NumJoysticks(); i++) {
                SDL_Joystick* joystick = SDL_JoystickOpen(i);
                joysticks.push_back(joystick);
            }
        }

        EVENTSYS.addListener(this, SDL_JOYAXISMOTION);
        EVENTSYS.addListener(this, SDL_JOYBUTTONDOWN);
        EVENTSYS.addListener(this, SDL_JOYBUTTONUP);

        EVENTSYS.addListener(this, SDL_KEYDOWN);
        EVENTSYS.addListener(this, SDL_KEYUP);
    }

    InputProvider::~InputProvider() {
        for (auto joystick : joysticks)
            SDL_JoystickClose(joystick);
    }

    bool InputProvider::isKeyDown(int keyCode) const {
        return keyboardState.keys[keyCode];
    }

    bool InputProvider::isKeyUp(int keyCode) const {
        return !keyboardState.keys[keyCode];
    }

    bool InputProvider::isKeyJustPressed(int keyCode) const {
        return keyboardState.keys[keyCode] && !oldKeyboardState.keys[keyCode];
    }

    bool InputProvider::isKeyJustReleased(int keyCode) const {
        return !keyboardState.keys[keyCode] && oldKeyboardState.keys[keyCode];
    }

    bool InputProvider::isJoystickPressedLeft() const {
        return joystickState.x_axis == -1;
    }

    bool InputProvider::isJoystickJustPressedLeft() const {
        return false;
    }

    bool InputProvider::isJoystickPressedRight() const {
        return joystickState.x_axis == 1;
    }

    bool InputProvider::isJoystickJustPressedRight() const {
        return false;
    }

    bool InputProvider::isJoystickPressedUp() const {
        return joystickState.y_axis == -1;
    }

    bool InputProvider::isJoystickJustPressedUp() const {
        return false;
    }

    bool InputProvider::isJoystickPressedDown() const {
        return joystickState.y_axis == 1;
    }

    bool InputProvider::isJoystickJustPressedDown() const {
        return false;
    }

    void InputProvider::onEvent(SDL_Event const& event) {
        switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.scancode < 256)
                    keyboardState.keys[event.key.keysym.scancode] = true;
                break;
            case SDL_KEYUP:
                if (event.key.keysym.scancode < 256)
                    keyboardState.keys[event.key.keysym.scancode] = false;
                break;
            case SDL_JOYAXISMOTION:
                if (event.jaxis.which == 0) {
                    if (event.jaxis.axis == 0) {
                        std::cout << "event.jaxis.value: " << event.jaxis.value << std::endl;
                        if (event.jaxis.value > 20000)
                            joystickState.x_axis = 1;
                        else if (event.jaxis.value < (-20000))
                            joystickState.x_axis = -1;
                        else
                            joystickState.x_axis = 0;
                    }

                    if (event.jaxis.axis == 1) {
                        if (event.jaxis.value > 20000)
                            joystickState.y_axis = 1;
                        else if (event.jaxis.value < -20000)
                            joystickState.y_axis = -1;
                        else
                            joystickState.y_axis = 0;
                    }
                }
                break;
        }
    }

    void InputProvider::copyNewStateToOldState() {
        oldKeyboardState = keyboardState;
        oldJoystickState = joystickState;
    }
}