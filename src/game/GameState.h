#pragma once

namespace Acidrain {

    template<class T>
    class GameState {
    public:

        virtual void onEnter(T*) = 0;

        virtual void onExit(T*) = 0;

        virtual void update(T*, float elapsedSeconds) = 0;

        virtual void render(T*, float alpha) = 0;
    };

    template<class T>
    class GameStateMachine {
    public:
        GameStateMachine(T* ownr)
                : owner(ownr), currentState(nullptr), previousState(nullptr), globalState(nullptr) {
        }

        void setCurrentState(GameState<T>* s) {
            currentState = s;
        }

        void setPreviousState(GameState<T>* s) {
            previousState = s;
        }

        void setGlobalState(GameState<T>* s) {
            globalState = s;
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
            previousState = currentState;
            if (currentState != nullptr)
                currentState->onExit(owner);

            currentState = s;
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
    };

} // namespace Acidrain