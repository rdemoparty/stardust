#pragma once

#include <easylogging++.h>

namespace Acidrain {

    template<class T>
    class GameState {
    public:

        virtual const char* name() const = 0;
        virtual void onEnter(T*) {};
        virtual void onExit(T*) {};
        virtual void update(T*, float elapsedSeconds) = 0;
        virtual void render(T*, float alpha) {};
    };

    template<class T>
    class GameStateMachine {
    public:
        GameStateMachine(T* fsmOwner)
                : owner(fsmOwner),
                  currentState(nullptr),
                  previousState(nullptr),
                  globalState(nullptr) {
        }

        GameState<T>* getCurrentState() {
            return currentState;
        }

        void setGlobalState(GameState<T>* s) {
            LOG(INFO) << "Setting global game state [" << getStateName(globalState) << "] ---------> [" << getStateName(s) << "]";

            if (globalState != nullptr)
                globalState->onExit(owner);

            globalState = s;
            if (globalState != nullptr)
                globalState->onEnter(owner);
        }

        void update(float elapsedMilliseconds) {
            if (globalState != nullptr)
                globalState->update(owner, elapsedMilliseconds);

            if (currentState != nullptr)
                currentState->update(owner, elapsedMilliseconds);
        }

        void render(float alpha) {
            if (globalState != nullptr)
                globalState->render(owner, alpha);

            if (currentState != nullptr)
                currentState->render(owner, alpha);
        }

        void changeState(GameState<T>* s) {
            LOG(INFO) << "Switching game state [" << getStateName(currentState) << "] ---------> [" << getStateName(s) << "]";

            previousState = currentState;
            if (currentState != nullptr)
                currentState->onExit(owner);

            currentState = s;
            if (currentState != nullptr)
                currentState->onEnter(owner);
        }

        void revertToPreviousState() {
            changeState(previousState);
        }

    private:
        T* owner;
        GameState<T>* currentState;
        GameState<T>* previousState;
        GameState<T>* globalState;

        const char* getStateName(GameState<T>* state) {
            return state == nullptr ? "null" : state->name();
        }
    };

} // namespace Acidrain