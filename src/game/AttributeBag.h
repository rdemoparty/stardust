#pragma once

#include <map>

namespace Acidrain {

    using namespace std;

    class AttributeBag {
    public:

        int getInt(const char* const key);

        void setInt(const char* const key, int value);

        float getFloat(const char* const key);

        void setFloat(const char* const key, float value);

    private:
        map<string, int> integerMap;
        map<string, float> floatMap;
    };
}