#include "AUI/Screen.h"
#include <algorithm>

namespace AUI {

Screen::Screen(const std::string& inDebugName)
: debugName{inDebugName}
, componentMap()
{
}

void Screen::registerComponent(Component& component)
{
    // If the key is empty or we already have a component with that key, fail.
    const entt::hashed_string& key = component.getKey();
    if ((key == entt::hashed_string{""})
        || (componentMap.find(key.value()) != componentMap.end())) {
        AUI_LOG_ERROR("Tried to add component with key that is empty or already in use. Screen: %s, Key: %s", debugName.c_str(), key.data());
    }

    // Add it to the map.
    componentMap[key.value()] = &component;
}

void Screen::unregisterComponent(const entt::hashed_string& key)
{
    // If we don't have a component with that key, fail.
    auto mapIt = componentMap.find(key.value());
    if (mapIt == componentMap.end()) {
        AUI_LOG_ERROR("No component with given key exists. Screen: %s, Key: %s", debugName.c_str(), key.data());
    }

    // Erase the pointer from the map.
    componentMap.erase(mapIt);
}

void Screen::registerListener(EventType eventType, Component* listener)
{
    std::vector<Component*>& listeners = listenerMap[eventType];
    listeners.push_back(listener);
}

void Screen::unregisterListener(EventType eventType, Component* listener)
{
    // Find the given listener.
    std::vector<Component*>& listeners = listenerMap[eventType];
    auto it = std::find(listeners.begin(), listeners.end(), listener);

    // If the listener was found, erase it.
    if (it != listeners.end()) {
        listeners.erase(it);
    }
    else {
        AUI_LOG_ERROR("Listener not found while trying to unregister.");
    }
}

bool Screen::handleEvent(SDL_Event& event)
{
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            return handleMouseButtonDown(event.button);
        }
        case SDL_MOUSEBUTTONUP: {
            return handleMouseButtonUp(event.button);
        }
        case SDL_MOUSEMOTION: {
            // Motion logic never counts the event as handled, since the mouse
            // might've gone into or come from a non-UI point on the screen.
            handleMouseMove(event.motion);
            return false;
        }
        case SDL_KEYDOWN: {
            return handleKeyDown(event.key);
        }
        case SDL_TEXTINPUT: {
            return handleTextInput(event.text);
        }
        default:
            break;
    }

    return false;
}

bool Screen::handleMouseButtonDown(SDL_MouseButtonEvent& event)
{
    bool eventHandled{false};
    for (Component* listener : listenerMap[EventType::MouseButtonDown]) {
        // If the listener isn't visible, ignore it.
        if (!(listener->getIsVisible())) {
            continue;
        }

        // Call the listener's callback and track if the event was handled.
        if (listener->onMouseButtonDown(event)) {
            eventHandled = true;
        }
    }

    return eventHandled;
}

bool Screen::handleMouseButtonUp(SDL_MouseButtonEvent& event)
{
    bool eventHandled{false};
    for (Component* listener : listenerMap[EventType::MouseButtonUp]) {
        // If the listener isn't visible, ignore the event..
        if (!(listener->getIsVisible())) {
            continue;
        }

        // Call the listener's callback and track if the event was handled.
        if (listener->onMouseButtonUp(event)) {
            eventHandled = true;
        }
    }

    return eventHandled;
}

void Screen::handleMouseMove(SDL_MouseMotionEvent& event)
{
    for (Component* listener : listenerMap[EventType::MouseMove]) {
        // If the listener isn't visible, ignore it.
        if (!(listener->getIsVisible())) {
            continue;
        }

        // Call the listener's callback.
        listener->onMouseMove(event);
    }
}

bool Screen::handleKeyDown(SDL_KeyboardEvent& event)
{
    bool eventHandled{false};
    for (Component* listener : listenerMap[EventType::KeyDown]) {
        // If the listener isn't visible, ignore the event..
        if (!(listener->getIsVisible())) {
            continue;
        }

        // Call the listener's callback and track if the event was handled.
        if (listener->onKeyDown(event)) {
            eventHandled = true;
        }
    }

    return eventHandled;
}

bool Screen::handleTextInput(SDL_TextInputEvent& event)
{
    bool eventHandled{false};
    for (Component* listener : listenerMap[EventType::TextInput]) {
        // If the listener isn't visible, ignore the event..
        if (!(listener->getIsVisible())) {
            continue;
        }

        // Call the listener's callback and track if the event was handled.
        if (listener->onTextInput(event)) {
            eventHandled = true;
        }
    }

    return eventHandled;
}

void Screen::render()
{
    // Rendering is left up to derived classes.
}

} // namespace AUI
