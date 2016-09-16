#pragma once

#include <memory>

namespace Acidrain {

    class Timer;
    class Stardust;

    using namespace std;

    class Application {
    public:

        explicit Application(int argc, char** argv);
        virtual ~Application();

        void run();

    private:
        void initializeSystems();
        void configureLogging();

        unique_ptr<Timer> timer;
        unique_ptr<Stardust> game;
    };

} // end of namespace Acidrain