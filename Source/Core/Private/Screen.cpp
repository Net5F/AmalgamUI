#include "AUI/Screen.h"
#include <algorithm>

namespace AUI {

Screen::Screen(const std::string& inDebugName)
: debugName{inDebugName}
, componentMap()
, currentHoveredComponent{nullptr}
, currentPressedComponent{nullptr}
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

            // Note: Leave must be called before Enter (see header comment).
            handleMouseLeave(event.motion);
            handleMouseEnter(event.motion);
            break;
        }
        default:
            break;
    }

    return false;
}

bool Screen::handleMouseButtonDown(SDL_MouseButtonEvent& event)
{
    if (currentPressedComponent != nullptr) {
        AUI_LOG_ERROR("Received a MOUSEBUTTONDOWN while a component is"
        " currently pressed.");
    }

    for (Component* listener : listenerMap[EventType::MouseButtonDown]) {
        if (listener->containsPoint({event.x, event.y})) {
            listener->onMouseButtonDown(event);
            currentPressedComponent = listener;
            return true;
        }
    }

    return false;
}

bool Screen::handleMouseButtonUp(SDL_MouseButtonEvent& event)
{
    // If a component is currently pressed.
    if (currentPressedComponent != nullptr) {
        // Check if the mouse is still over the pressed component or not.
        bool isHovered{false};
        if (currentPressedComponent == currentHoveredComponent) {
            isHovered = true;
        }

        // Release the component.
        currentPressedComponent->onMouseButtonUp(event, isHovered);
        currentPressedComponent = nullptr;
    }

    return true;
}

void Screen::handleMouseMove(SDL_MouseMotionEvent& event)
{
    // If the mouse entered a listening component, give it the event.
    for (Component* listener : listenerMap[EventType::MouseMove]) {
        if (listener->containsPoint({event.x, event.y})) {
            listener->onMouseMove(event);
        }
    }
}

void Screen::handleMouseLeave(SDL_MouseMotionEvent& event)
{
    // If we have a current hovered component, check if the mouse left it.
    if ((currentHoveredComponent != nullptr)
    && !(currentHoveredComponent->containsPoint({event.x, event.y}))) {
        // Unhover the current hovered component.
        currentHoveredComponent->onMouseLeave(event);
        currentHoveredComponent = nullptr;
    }
}

void Screen::handleMouseEnter(SDL_MouseMotionEvent& event)
{
    // If we're still hovering a previous hovered component, skip checking
    // for a new one.
    if (currentHoveredComponent != nullptr) {
        return;
    }

    // Check if we're entering a new component.
    for (Component* listener : listenerMap[EventType::MouseEnter]) {
        // If we just moved the mouse over this component and it isn't already
        // hovered.
        if ((listener != currentHoveredComponent)
        && listener->containsPoint({event.x, event.y})) {
            // A new component is becoming hovered.
            // If we have a current hovered component, tell the listener we're
            // leaving.
            if (currentHoveredComponent != nullptr) {
                currentHoveredComponent->onMouseLeave(event);
            }

            // Call the listener's callback and save the hovered component.
            listener->onMouseEnter(event);
            currentHoveredComponent = listener;
        }
    }
}

void Screen::render()
{
    // Rendering is left up to derived classes.
}

} // namespace AUI
