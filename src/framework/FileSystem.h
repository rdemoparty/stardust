#pragma once

#include <string>
#include <CommandLineParser.h>

#define FILESYS Acidrain::FileSystem::getInstance()


namespace Acidrain {

    DECLARE_string(data_dir);

    using namespace std;

    struct FileInfo {
        string filename;
        string uri;
        bool isDirectory;
    };

    class FileSystem {
    public:

        static FileSystem& getInstance();

        ~FileSystem();

        void init(std::string rootDir);

        std::string absolutePath(const char* relativePath);

        std::string absolutePath(std::string relativePath);

        static std::string getExePath();

        static std::string getExeDir(std::string fullPath);

        std::string getFileContent(const char* pathRelativeToRoot);

        std::string getFileContent(const std::string& pathRelativeToRoot);

        vector<FileInfo> filesInDirectory(string directoryURI);

        vector<FileInfo> filesInDirectoryRecursive(string directoryURI, vector<FileInfo> filesSoFar = {});

    private:
        std::string rootDir = "./";

    };

} // namespace Acidrain

