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

        void init(string rootDir);

        string absolutePath(const char* relativePath);

        string absolutePath(string relativePath);

        static string getExePath();

        static string getExeDir(string fullPath);

        bool fileExists(const string& pathRelativeToRoot);

        string getFileContent(const char* pathRelativeToRoot);

        string getFileContent(const string& pathRelativeToRoot);

        vector<FileInfo> filesInDirectory(string directoryURI);

        vector<FileInfo> filesInDirectoryRecursive(string directoryURI, vector<FileInfo> filesSoFar = {});

    private:
        string rootDir = "./";
    };

} // namespace Acidrain

