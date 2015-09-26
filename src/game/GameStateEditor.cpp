#include <easylogging++.h>
#include <GameStateEditor.h>
#include <Stardust.h>
#include <Font.h>
#include <GfxSystem.h>
#include <InputProvider.h>
#include <fstream>
#include <mongoose.h>
#include <CommandLineParser.h>
#include <FileSystem.h>
#include <sstream>
#include <StringUtils.h>
#include <Level.h>
#include <Scene.h>
#include <GameStatePreviewEntity.h>
#include <GameStatePreviewLevel.h>
#include <Version.h>

namespace Acidrain {

    GameStateEditor& GameStateEditor::instance() {
        static GameStateEditor instance;
        return instance;
    }

    inline bool isPostRequest(struct mg_connection* conn) {
        return strcmp(conn->request_method, "POST") == 0;
    }

    inline bool isGetRequest(struct mg_connection* conn) {
        return strcmp(conn->request_method, "GET") == 0;
    }

    inline bool stringStartsWith(string& value, string prefix) {
        return value.find(prefix) == 0;
    }

    void persistFile(const string& path, const string& content) {
        ofstream out;
        out.open(path, ios :: out | ios :: binary);
        out << content;
        out.close();
    }

    struct MultipartEntry {
        string var;
        string filename;
        string content;
        bool isFile;
    };

    map<string, MultipartEntry> parseMultipart(struct mg_connection* conn) {
        map<string, MultipartEntry> result;

        const char* data;
        int dataLength, offset = 0;
        char varName[100], filename[100];

        while ((offset = mg_parse_multipart(
                conn->content + offset,
                conn->content_len - offset,
                varName, sizeof(varName),
                filename, sizeof(filename),
                &data, &dataLength)) > 0) {

            MultipartEntry entry;
            entry.var = varName;
            entry.filename = filename;
            entry.content = string(data, dataLength);
            entry.isFile = !entry.filename.empty();

            result[entry.var] = entry;
        }

        return result;
    }

    static Stardust* theGame = nullptr;

    static int handleRequest(struct mg_connection* conn) {
        string URI = conn->uri;

        stringstream sstream;
        sstream << "{\"status\": {\"code\": \"OK\"}, \"result\": [";

        if (stringStartsWith(URI, "/browse")) {
            string startDir = URI.substr(7); // remove "/browse", forward and trailing slash
            if (startDir[startDir.length() - 1] == '/')
                startDir = startDir.substr(0, startDir.length() - 1);
            if (startDir[0] == '/')
                startDir = startDir.substr(1);

            vector<FileInfo> files = FILESYS.filesInDirectoryRecursive(startDir);
            bool isFirst = true;
            for (auto& f : files) {
                if (isFirst)
                    isFirst = false;
                else
                    sstream << ", ";

                sstream << "{\"filename\": \"" << f.uri << "\", \"isDir\": " << (f.isDirectory ? "true" : "false") << "}";
            }
            sstream << "]}";

            mg_send_header(conn, "Content-Type", "application/json");
            mg_printf_data(conn, "%s", sstream.str().c_str());

            return MG_TRUE;
        }

        if (stringStartsWith(URI, "/data")) {
            if (isGetRequest(conn)) {
                string absolutePath = FILESYS.absolutePath(URI.substr(6)); // remove /data/ from URI
                mg_send_file(conn, absolutePath.c_str(), nullptr);
                return MG_MORE;
            } else if (isPostRequest(conn)) {

                LOG(INFO) << "POST request on /data with multiparts: ";
                LOG(TRACE) << conn->content;
                map<string, MultipartEntry> multiparts = parseMultipart(conn);
                for (auto& kv : multiparts)
                    LOG(INFO) << "\t" << "{var: " << kv.second.var << ", filename: " << kv.second.filename << ", size: " << kv.second.content.size() << ", isFile: " << kv.second.isFile << "}";

                // extract the dir in which to save uploaded files
                string dir;
                if (multiparts.find("dir") != multiparts.end())
                    dir = multiparts["dir"].content;

                // save files with their original filename
                for (auto& kv : multiparts) {
                    auto& part = kv.second;
                    if (part.isFile) {
                        const string filePath = FILESYS.absolutePath(dir + "/" + part.filename);
                        LOG(INFO) << "Saving file " << part.filename << " of " << part.content.size() << " bytes to " << filePath;
                        persistFile(filePath, part.content);
                    }
                }

                mg_send_header(conn, "Content-Type", "application/json");
                mg_printf_data(conn, "{\"status\": {\"code\": \"OK\"}}");

                return MG_TRUE;
            }
        }

        if (stringStartsWith(URI, "/editor/preview-entity")) {
            vector<string> pieces = StringUtils::split(URI, '/');
            string entityName = pieces.at(pieces.size() - 1);
            if (theGame != nullptr) {
                LOG(INFO) << "Previewing entity " << entityName;
                GameStatePreviewEntity::instance().previewEntity(entityName);
                theGame->fsm->changeState(&GameStatePreviewEntity::instance());
            }

            mg_send_header(conn, "Content-Type", "application/json");
            mg_printf_data(conn, "{\"status\": {\"code\": \"OK\"}}");

            return MG_TRUE;
        }

        if (stringStartsWith(URI, "/editor/preview-level")) {
            vector<string> pieces = StringUtils::split(URI, '/');
            string levelName = pieces.at(pieces.size() - 1);
            if (theGame != nullptr) {
                LOG(INFO) << "Previewing level " << levelName;
                GameStatePreviewLevel::instance().preview(theGame, levelName);
                theGame->fsm->changeState(&GameStatePreviewLevel::instance());
            }

            mg_send_header(conn, "Content-Type", "application/json");
            mg_printf_data(conn, "{\"status\": {\"code\": \"OK\"}}");

            return MG_TRUE;
        }

        return MG_FALSE;
    }

    static int webEventHandler(struct mg_connection* conn, enum mg_event ev) {
        switch (ev) {
            case MG_AUTH:
                return MG_TRUE;
            case MG_REQUEST:
                return handleRequest(conn);
            default:
                return MG_FALSE;
        }
    }

    static struct mg_server* server = nullptr;

    void GameStateEditor::onEnter(Stardust* game) {
        theGame = game;
        if (!font)
            font = make_shared<Font>("fonts/Neo Sans Pro Bold.ttf", 20.0f);

        server = mg_create_server(nullptr, webEventHandler);

//        string documentRoot = FILESYS.getExeDir(FILESYS.getExePath()) + "/editor/web/";
//        string documentRoot = FILESYS.getExeDir(FILESYS.getExeDir(FILESYS.getExePath())) + "/tools/editor/web/";
        string documentRoot = "../tools/editor/web/";
        LOG(INFO) << "Starting web server with document root " << documentRoot;

        mg_set_option(server, "document_root", documentRoot.c_str());     // Serve current directory
        mg_set_option(server, "listening_port", "127.0.0.1:8080");  // Open port 8080

        GFXSYS.setClearColor(vec3(0, 0, 0));
    }

    void GameStateEditor::onExit(Stardust* game) {
        mg_destroy_server(&server);
        theGame = nullptr;
    }

    void GameStateEditor::update(Stardust* game, float elapsedSeconds) {
        if (INPUT.isKeyJustPressed(SDL_SCANCODE_ESCAPE) && game->fsm->getCurrentState() == nullptr) {
            game->quitGame = true;
        }

        mg_poll_server(server, 50);
    }

    void GameStateEditor::render(Stardust* game, float alpha) {
        if (game->fsm->getCurrentState() == nullptr) {
            GFXSYS.clearScreen();

            GFXSYS.setTransparencyMode(TransparencyMode::Additive);
            font->print(100, 100, "Editor mode. Waiting for commands", vec4(1, 1, 1, 0.8f));
            font->print(100, 132, string("Version ") + STARDUST_VERSION , vec4(1, 1, 1, 0.8f));

            GFXSYS.show();
        }
    }
} // namespace Acidrain