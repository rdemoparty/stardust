#include <GameSession.h>
#include <sstream>
#include <FileSystem.h>
#include <easylogging++.h>

namespace Acidrain {

    static const int MAX_LEVELS = 20;

    string getLevelPath(int level) {
        stringstream levelUri;
        levelUri << "levels/level" << level << ".json";
        return levelUri.str();
    }

    GameSession::GameSession() {
        // Search for the last level. Levels' filenames must be in order, e.g: level1.json, level2.json, level3.json
        lastLevel = 1;
        for (int i = 1; i <= MAX_LEVELS; i++) {
            string&& levelName = getLevelPath(i);
            if (FILESYS.fileExists(levelName)) {
                LOG(INFO) << "Detected level " << levelName;
                lastLevel = i;
            } else {
                break;
            }
        }
        reset();
    }

    GameSession::~GameSession() {
    }

    void GameSession::reset() {
        currentLevel = 1;
        livesRemaining = 3;
        score = 0;
        sessionAttributes.clear();
        state = GameSessionState::NEW;
    }

    int GameSession::getRemainingLives() {
        return livesRemaining;
    }

    bool GameSession::isGameCompleted() {
        return currentLevel > lastLevel;
    }

    void GameSession::notifyPlayerDeath() {
        livesRemaining--;
        if (livesRemaining < 1) {
            state = GameSessionState::GAME_OVER;
        }
    }

    void GameSession::notifyLevelFinish() {
        currentLevel++;
        if (currentLevel > lastLevel) {
            state = GameSessionState::GAME_FINISHED;
        }
    }

    int GameSession::getScore() {
        return score;
    }

    void GameSession::notifyIncrementScore(int amount) {
        score += amount;
    }

    GameSessionState GameSession::getState() const {
        return state;
    }

    string GameSession::getLevelUri() {
        return getLevelPath(currentLevel);
    }

    void GameSession::notifySessionStarted() {
        state = GameSessionState::PLAYING;
    }
} // namespace Acidrain