#include <ScriptExporterRegistry.h>
#include <vector>
#include <ScriptExporter.h>

using namespace std;

namespace Acidrain {

    class ScriptExporterImpl {
    public:
        static ScriptExporterImpl& getInstance();

        void add(const ScriptExporter* exporter);
        void exportToScript(lua_State* L);

        vector<const ScriptExporter*> exporters;
    };

    void ScriptExporterRegistry::add(const ScriptExporter* exporter) {
        ScriptExporterImpl::getInstance().add(exporter);
    }

    void ScriptExporterRegistry::exportTo(lua_State* L) {
        ScriptExporterImpl::getInstance().exportToScript(L);
    }

    // ----------------------------------------------------------------------

    ScriptExporterImpl& ScriptExporterImpl::getInstance() {
        static ScriptExporterImpl instance;
        return instance;
    }

    void ScriptExporterImpl::add(const ScriptExporter* exporter) {
        exporters.push_back(exporter);
    }

    void ScriptExporterImpl::exportToScript(lua_State* L) {
        for (const auto& exporter : exporters) {
            exporter->exportToScript(L);
        }
    }

} // end of namespace Acidrain
