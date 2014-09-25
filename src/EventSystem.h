#pragma once

#include <map>
#include <vector>
#include <SDL2/SDL.h>

#define EVENTSYS Acidrain::EventSystem::getInstance()

namespace Acidrain {

    class EventListener {
    public:
        virtual void onEvent(const SDL_Event &) = 0;
    };

    typedef std::vector<EventListener *> EventListenerList;
    typedef std::pair<Uint32, EventListenerList *> EventListenerPair;
    typedef std::map<Uint32, EventListenerList *> EventListenerMap;

    class EventSystem {
    public:

        static EventSystem &getInstance();

        void init();

        void update();

        void addListener(EventListener *listener, Uint32 eventTypeToListenTo);

        void removeListener(EventListener *listener);

    private:
        EventSystem();

        ~EventSystem();

        EventListenerMap listeners;
    };


} // namespace Acidrain