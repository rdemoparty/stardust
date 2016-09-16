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

    // Search for the last level. Level file names must be in alphanumeric order, e.g: level1.json, level2.json, level3.json
    int detectLastLevel() {
        int lastLevel = 1;
        for (int i = 1; i <= MAX_LEVELS; i++) {
            string&& levelName = getLevelPath(i);
            if (FILESYS.fileExists(levelName)) {
                LOG(INFO) << "Detected level " << levelName;
                lastLevel = i;
            } else {
                break;
            }
        }
        return lastLevel;
    }

    GameSession::GameSession() {
        lastLevel = detectLastLevel();
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

    int GameSession::getRemainingLives() const {
        return livesRemaining;
    }

    bool GameSession::isGameCompleted() const {
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

    int GameSession::getScore() const {
        return score;
    }

    void GameSession::notifyIncrementScore(int amount) {
        score += amount;
    }

    GameSessionState GameSession::getState() const {
        return state;
    }

    string GameSession::getCurrentLevelUri() const {
        return getLevelPath(currentLevel);
    }

    void GameSession::notifySessionStarted() {
        state = GameSessionState::PLAYING;
    }

    int GameSession::getCurrentLevel() const {
        return currentLevel;
    }

    bool GameSession::needToWatchLevelIntro() const {
        if (watchedIntros.find(currentLevel) == watchedIntros.end()) {
            return true;
        } else {
            return !watchedIntros.at(currentLevel);
        }
    }

    bool GameSession::needToWatchLevelOutro() const {
        if (watchedOutros.find(currentLevel) == watchedOutros.end()) {
            return true;
        } else {
            return !watchedOutros.at(currentLevel);
        }
    }

    void GameSession::levelIntroWatched() {
        watchedIntros[currentLevel] = true;
    }

    void GameSession::levelOutroWatched() {
        watchedOutros[currentLevel] = true;
    }

} // namespace Acidrain