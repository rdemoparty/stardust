#include <EventSystem.h>
#include <SDL2/SDL.h>
#include <assert.h>
#include <algorithm>

namespace Acidrain {

    EventSystem &EventSystem::getInstance() {
        static EventSystem instance;
        return instance;
    }

    EventSystem::EventSystem() {
    }

    EventSystem::~EventSystem() {
        for (auto it : listeners)
            delete it.second;
    }

    void EventSystem::init() {
        SDL_Init(SDL_INIT_EVERYTHING);
    }

    void EventSystem::addListener(EventListener *listener, Uint32 eventTypeToListenTo) {
        assert(NULL != listener);

        EventListenerMap::iterator it = listeners.find(eventTypeToListenTo);
        if (it == listeners.end())
            listeners.insert(EventListenerPair(eventTypeToListenTo, new EventListenerList()));

        listeners[eventTypeToListenTo]->push_back(listener);
    }

    void EventSystem::removeListener(EventListener *listener) {
        assert(NULL != listener);

        for (auto it : listeners) {
            EventListenerList &listOfListeners = *it.second;
            auto lit = find(begin(listOfListeners), end(listOfListeners), listener);
            if (lit != listOfListeners.end())
                listOfListeners.erase(lit);
        }
    }

    void EventSystem::update() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            auto listenersForThisEvent = listeners.find(event.type);
            if (listenersForThisEvent != listeners.end())
                for (auto listener : *(listenersForThisEvent->second))
                    listener->onEvent(event);
        }
    }

} // namespace Acidrain