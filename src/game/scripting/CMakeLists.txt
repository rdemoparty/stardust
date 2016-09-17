set(HEADERS
        ${HEADERS}

        ${CMAKE_CURRENT_SOURCE_DIR}/scripting/AudioScriptExporter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/scripting/BasicScriptExporter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/scripting/CameraScriptExporter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/scripting/CutSceneScriptExporter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/scripting/DialogRepositoryScriptExporter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/scripting/EntityScriptExporter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/scripting/GameSessionScriptExporter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/scripting/InputScriptExporter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/scripting/SceneScriptExporter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/scripting/ScriptingSystem.h
        ${CMAKE_CURRENT_SOURCE_DIR}/scripting/Script.h
        ${CMAKE_CURRENT_SOURCE_DIR}/scripting/ScriptUtils.h

        PARENT_SCOPE
        )
set(SOURCE
        ${SOURCE}

        ${CMAKE_CURRENT_SOURCE_DIR}/AudioScriptExporter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/BasicScriptExporter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CameraScriptExporter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CutSceneScriptExporter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DialogRepositoryScriptExporter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/EntityScriptExporter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameSessionScriptExporter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/InputScriptExporter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SceneScriptExporter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ScriptingSystem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Script.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ScriptUtils.cpp

        PARENT_SCOPE
        )
