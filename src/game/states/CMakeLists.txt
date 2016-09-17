set(HEADERS
        ${HEADERS}

        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateCredits.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateCutSceneAfterLevel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateCutSceneBeforeLevel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateEditor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateGameFinished.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateGameOver.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateHandlePlayerDeath.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateIntro.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateLevelFinished.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateMenu.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateNewGame.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateNextLevel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateOptions.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStatePlayLevel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStatePreviewEntity.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStatePreviewLevel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateShowLevelStats.h
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateTest.h

        PARENT_SCOPE
        )

set(SOURCE
        ${SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateCredits.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateCutSceneAfterLevel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateCutSceneBeforeLevel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateGameFinished.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateGameOver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateHandlePlayerDeath.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateIntro.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateLevelFinished.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateEditor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateMenu.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateNewGame.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateNextLevel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateOptions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStatePlayLevel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStatePreviewEntity.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStatePreviewLevel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateShowLevelStats.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameStateTest.cpp

        PARENT_SCOPE
        )
