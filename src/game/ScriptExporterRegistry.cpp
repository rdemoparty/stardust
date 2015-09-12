#include <ScriptExporterRegistry.h>
#include <vector>
#include <ScriptExporter.h>

using namespace std;

namespace Acidrain {

    static vector<const ScriptExporter*> exporters;

    void ScriptExporterRegistry::add(const ScriptExporter *exporter) {
        exporters.push_back(exporter);
    }

    void ScriptExporterRegistry::exportTo(lua_State *L) {
        for (const auto& exporter : exporters) {
            exporter->exportToScript(L);
        }
    }
} // end of namespace Acidrain
