#pragma once

#include <map>
#include <iostream>
#include <cstring>

namespace Acidrain {

    using namespace std;

    class AttributeBag {
    public:

        int getInt(const char* const key);

        void setInt(const char* const key, int value);

        float getFloat(const char* const key);

        void setFloat(const char* const key, float value);

        void clear();

    private:

        // We need this otherwise the map will compare the pointers themselves and not the
        // text they point to. The effect in this case is having multiple entries for the same
        // parameter name. Check issue #6 for more details.
        struct StringComparator {
            bool operator()(char const* a, char const* b) {
                return std::strcmp(a, b) < 0;
            }
        };

        map<const char*, int, StringComparator> integerMap;
        map<const char*, float, StringComparator> floatMap;
    };
}
