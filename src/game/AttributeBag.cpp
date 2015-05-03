#include <AttributeBag.h>

namespace Acidrain {
    int AttributeBag::getInt(const char* const key) {
        return integerMap[key];
    }

    void AttributeBag::setInt(const char* const key, int value) {
        integerMap[key] = value;
    }

    float AttributeBag::getFloat(const char* const key) {
        return floatMap[key];
    }

    void AttributeBag::setFloat(const char* const key, float value) {
        floatMap[key] = value;
    }

    void AttributeBag::clear() {
        floatMap.clear();
        integerMap.clear();
    }
}