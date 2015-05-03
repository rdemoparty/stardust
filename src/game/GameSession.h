#pragma once

#include <AttributeBag.h>

namespace Acidrain {

    /**
     * A game session contains, as its name says, all needed context for a player's gaming session.
     * It can tell for instance which is the current playing level, how many points has the player
     * gathered so far, how many lives the player has and also holds any badges or records the owning
     * player might have won.
     */

    enum class GameSessionState {
        NEW,
        PLAYING,
        GAME_OVER,
        GAME_FINISHED
    };

    class GameSession {
    public:
        explicit GameSession();
        virtual ~GameSession();

        void reset();

        GameSessionState getState() const;

        int getRemainingLives();
        int getScore();
        bool isGameCompleted();

        void notifyIncrementScore(int amount);
        void notifyPlayerDeath();
        void notifyLevelFinish();

        string getLevelUri();

        void notifySessionStarted();

    private:
        int currentLevel;
        int lastLevel;
        int livesRemaining;
        int score;
        GameSessionState state = GameSessionState::NEW;
        AttributeBag sessionAttributes;
    };

} // namespace Acidrain