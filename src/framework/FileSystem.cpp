#include <FileSystem.h>
#include <fstream>
#include <easylogging++.h>
#include <algorithm>
#include <sys/stat.h>

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
    #include <tchar.h>
#else
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#endif


namespace Acidrain {

    DEFINE_string(data_dir, d, "Data dir relative to cwd", "../data")

    FileSystem& FileSystem::getInstance() {
        static FileSystem instance;
        return instance;
    }

    void FileSystem::init(string root) {
        LOG(INFO) << "Initializing file system with root dir " << root;
        rootDir = root;
    }

    string FileSystem::absolutePath(const char* relativePath) {
        string absolutePath = rootDir + "/" + relativePath;
        LOG(TRACE) << "relative path [" << relativePath << "] expanded to [" << absolutePath << "]";
        return absolutePath;
    }

    string FileSystem::absolutePath(string relativePath) {
        return absolutePath(relativePath.c_str());
    }

#ifdef __APPLE__

	#include <mach-o/dyld.h>
	string FileSystem::getExePath() {
		char path[1024];
		uint32_t size = sizeof(path);
		if (_NSGetExecutablePath(path, &size) == 0)
			return string(path, size);
		else
			// throw exception("_NSGetExecutablePath buffer too small.");
            return string("invalid-exe-path");
	}

    string FileSystem::getExeDir(string fullPath) {
        char* path = strdup(fullPath.c_str());
        char* dname = dirname(path);

        string returnValue(dname, strlen(dname));

        free(path);
        return returnValue;
    }

#elif defined _WIN32 || defined _WIN64
	string FileSystem::getExePath() {
        char strFileName[1024];
        GetModuleFileName(NULL, strFileName, 1024);

        string s = strFileName;
        return strFileName;
	}

	string FileSystem::getExeDir(string fullPath) {
        size_t n = fullPath.rfind('\\');
        if (n == fullPath.npos) return "";

        fullPath.erase(n, fullPath.length() - n + 1);
        return fullPath;
	}
#elif defined __linux__

    string FileSystem::getExePath() {
        static char linkName[100];
        static char fullPath[256];

        pid_t pid = getpid();
        snprintf(linkName, sizeof(linkName), "/proc/%d/exe", pid);

        ssize_t ret = readlink(linkName, fullPath, sizeof(fullPath));
        if (-1 == ret)
            LOG(FATAL) << "readlink() failed while trying to get exe path";

        if (ret >= static_cast<int>(sizeof(fullPath)))
            LOG(FATAL) << "readlink() needs more memory for fullPath";

        return string(fullPath, ret);
    }

    string FileSystem::getExeDir(string fullPath) {
        char* path = strdup(fullPath.c_str());
        char* dname = dirname(path);

        string returnValue(dname, strlen(dname));

        free(path);
        return returnValue;
    }

#else
#error "Unknown platform"
#endif

    string FileSystem::getFileContent(const string& pathRelativeToRoot) {
        return getFileContent(pathRelativeToRoot.c_str());
    }

    string FileSystem::getFileContent(const char* pathRelativeToRoot) {
        string filename = absolutePath(pathRelativeToRoot);

        string contents;
        ifstream in(filename, ios::in | ios::binary);
        if (in) {
            in.seekg(0, ios::end);
            contents.resize(in.tellg());
            in.seekg(0, ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
        } else {
            LOG(FATAL) << "Failed reading content for file " << filename << ". Errno=" << errno;
        }
        LOG(TRACE) << "Loaded file " << filename << " with size " << contents.size();
        return contents;
    }


    FileSystem::~FileSystem() {
        LOG(INFO) << "Shutting down file system";
    }

#if defined _WIN32 || defined _WIN64

        vector<FileInfo> FileSystem::filesInDirectory(string directoryURI) {
            vector<FileInfo> result;

            HANDLE dir;
            WIN32_FIND_DATA fileData;
            string fileName;
            if ((dir = FindFirstFile(_T((absolutePath(directoryURI) +  "/*").c_str()), &fileData)) == INVALID_HANDLE_VALUE) {
                return result;
            }

            while (FindNextFile(dir, &fileData)) {
                fileName = fileData.cFileName;
                if (fileName !=  "." && fileName != "..") {
                    FileInfo info;
                    info.filename = fileName;
                    info.uri = (directoryURI.empty() ? "" : directoryURI + "/") + info.filename;
                    info.isDirectory = fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
                    result.push_back(info);
                }
            }

            return result;
        }
    #else
    vector<FileInfo> FileSystem::filesInDirectory(string directoryURI) {
        vector<FileInfo> result;

        DIR* dir = opendir(absolutePath(directoryURI).c_str());
        if (dir != nullptr) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                FileInfo info;
                info.filename = entry->d_name;
                info.uri = (directoryURI.empty() ? "" : directoryURI + "/") + info.filename;
                info.isDirectory = entry->d_type == DT_DIR;
                result.push_back(info);
            }
            closedir(dir);
        }

        return result;
    }
#endif

    vector<FileInfo> FileSystem::filesInDirectoryRecursive(string directoryURI, vector<FileInfo> filesSoFar) {
        vector<FileInfo> result = filesSoFar;

        vector<FileInfo> filesInThisDir = filesInDirectory(directoryURI);
        for (auto& f : filesInThisDir) {
            if (f.filename == "." || f.filename == "..") {
                continue;
            } else {
                result.push_back(f);
                if (f.isDirectory) {
                    string subdir = directoryURI.empty() ?
                                    f.filename :
                                    directoryURI + "/" + f.filename;
                    vector<FileInfo> files = filesInDirectoryRecursive(subdir, filesSoFar);
                    copy(files.begin(), files.end(), back_inserter(result));
                }
            }
        }

        return result;

    }

    bool FileSystem::fileExists(const string& pathRelativeToRoot) {
        string absoluteFilename = absolutePath(pathRelativeToRoot);

        struct stat buffer;
        return (stat (absoluteFilename.c_str(), &buffer) == 0);
    }
} // namespace Acidrain