#include <FileSystem.h>
#include <fstream>

#ifdef _WIN32
	#include <windows.h> // getmodulefilename
    #include <stdio.h>
    #include <psapi.h>
#else

#include <libgen.h> // dirname
#include <sys/resource.h>
#include <cstdio> // for snprintf
#include <string.h> // for strdup(), strlen()
#include <stdlib.h> // free()

#endif

#ifdef _WIN32
#else

#include <sys/types.h>
#include <unistd.h>

#endif

#include <iostream>

namespace Acidrain {

    FileSystem& FileSystem::getInstance() {
        static FileSystem instance;
        return instance;
    }

    void FileSystem::init(std::string root) {
        rootDir = root;
    }

    std::string FileSystem::absolutePath(const char* relativePath) {
        std::string absolutePath = rootDir + "/" + relativePath;
        std::cout << "Relative path [" << relativePath << "] expanded to [" << absolutePath << "]" << std::endl;
        return absolutePath;
    }

    std::string FileSystem::absolutePath(std::string relativePath) {
        return absolutePath(relativePath.c_str());
    }

#ifdef __APPLE__

	#include <mach-o/dyld.h>
	std::string FileSystem::getExePath() {
		char path[1024];
		uint32_t size = sizeof(path);
		if (_NSGetExecutablePath(path, &size) == 0)
			return std::string(path, size);
		else
			// throw std::exception("_NSGetExecutablePath buffer too small.");
            return std::string("invalid-exe-path");
	}

    std::string FileSystem::getExeDir(std::string fullPath) {
        char* path = strdup(fullPath.c_str());
        char* dname = dirname(path);

        std::string returnValue(dname, strlen(dname));

        free(path);
        return returnValue;
    }

#elif defined _WIN32 || defined _WIN64
	std::string FileSystem::getExePath() {
        char strFileName[1024];
        GetModuleFileName(NULL, strFileName, 1024);

        std::string s = strFileName;
        return strFileName;
	}

	std::string FileSystem::getExeDir(std::string fullPath) {
        int n = fullPath.rfind('\\');
        if (n == fullPath.npos) return "";

        fullPath.erase(n, fullPath.length() - n + 1);
        return fullPath;
	}
#elif defined __linux__

    std::string FileSystem::getExePath() {
        static char linkName[100];
        static char fullPath[256];

        pid_t pid = getpid();
        snprintf(linkName, sizeof(linkName), "/proc/%d/exe", pid);

        int ret = readlink(linkName, fullPath, sizeof(fullPath));
//        if (-1 == ret)
//            throw Exception("readlink() failed while trying to get exe path");
//
//        if (ret >= static_cast<int>(sizeof(fullPath)))
//            throw Exception("readlink() needs more memory for fullPath");

        return std::string(fullPath, ret);
    }

    std::string FileSystem::getExeDir(std::string fullPath) {
        char* path = strdup(fullPath.c_str());
        char* dname = dirname(path);

        std::string returnValue(dname, strlen(dname));

        free(path);
        return returnValue;
    }

#else
#error "Unknown platform"
#endif

    std::string FileSystem::getFileContent(const std::string& pathRelativeToRoot) {
        return getFileContent(pathRelativeToRoot.c_str());
    }

    std::string FileSystem::getFileContent(const char* pathRelativeToRoot) {
        std::string filename = absolutePath(pathRelativeToRoot);

        std::string contents;
        std::ifstream in(filename, std::ios::in | std::ios::binary);
        if (in) {
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
        } else {
//        throw(errno);
        }
        std::cout << "Loaded file [" << filename << "] with size " << contents.size() << std::endl;

        return contents;
    }


} // namespace Acidrain