#include <StringUtils.h>
#include <sstream>

namespace Acidrain {
    namespace StringUtils {

        vector<string> split(const string& str, char delimiter) {
            vector<string> result;

            istringstream f(str);
            string value;
            while (getline(f, value, delimiter)) {
                result.push_back(value);
            }

            return result;
        }

    }
}