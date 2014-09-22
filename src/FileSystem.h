#pragma once

#include <string>
#include <fstream>

namespace Acidrain {

    class FileSystem {
    public:

        static std::string getFileContent(const char *filename) {
            std::string contents;
            std::ifstream in(filename, std::ios::in | std::ios::binary);
            if (in) {
                in.seekg(0, std::ios::end);
                contents.resize(in.tellg());
                in.seekg(0, std::ios::beg);
                in.read(&contents[0], contents.size());
                in.close();
            }
            // throw exception with errno
            return contents;
        };

    };

} // namespace Acidrain

