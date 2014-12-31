#pragma once

#include <string>

#define FILESYS Acidrain::FileSystem::getInstance()

namespace Acidrain {

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

    private:
        std::string rootDir = "./";

    };

} // namespace Acidrain

