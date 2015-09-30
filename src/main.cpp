#include <Application.h>
#include <easylogging++.h>

using namespace Acidrain;

int main(int argc, char** argv) {
    try {
        Application application(argc, argv);
        application.run();
        LOG(INFO) << "Application finished. Exiting main()";
    } catch (std::exception* e) {
        LOG(FATAL) << e->what();
    }
    return 0;
}

