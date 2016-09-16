#include <ScriptingSystem.h>
#include <ScriptExporterRegistry.h>
#include <easylogging++.h>

#include <DialogRepositoryScriptExporter.h>
#include <BasicScriptExporter.h>
#include <GameSessionScriptExporter.h>
#include <EntityScriptExporter.h>
#include <InputScriptExporter.h>
#include <AudioScriptExporter.h>
#include <SceneScriptExporter.h>
#include <CameraScriptExporter.h>
#include <CutSceneScriptExporter.h>
#include <Script.h>

namespace Acidrain {

    ScriptingSystem& ScriptingSystem::getInstance() {
        static ScriptingSystem instance;
        return instance;
    }

    void ScriptingSystem::init() {
        LOG(INFO) << "Initializing scripting system";

        ScriptExporterRegistry::add(&BasicScriptExporter::getInstance());
        ScriptExporterRegistry::add(&GameSessionScriptExporter::getInstance());
        ScriptExporterRegistry::add(&DialogRepositoryScriptExporter::getInstance());
        ScriptExporterRegistry::add(&EntityScriptExporter::getInstance());
        ScriptExporterRegistry::add(&InputScriptExporter::getInstance());
        ScriptExporterRegistry::add(&AudioScriptExporter::getInstance());
        ScriptExporterRegistry::add(&SceneScriptExporter::getInstance());
        ScriptExporterRegistry::add(&CameraScriptExporter::getInstance());
        ScriptExporterRegistry::add(&CutSceneScriptExporter::getInstance());

        // TODO: find a better place to perform cutscenes loading
        new Script("cutscenes/test.cutscene.lua");
    }

    ScriptingSystem::ScriptingSystem() {
    }

    ScriptingSystem::~ScriptingSystem() {
        LOG(INFO) << "Shutting down scripting system";
    }
}