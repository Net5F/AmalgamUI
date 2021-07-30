#include "AUI/Screen.h"
#include <algorithm>

namespace AUI {

Screen::Screen(const std::string& inDebugName)
: debugName{inDebugName}
{
}

void Screen::registerListener(InternalEvent::Type eventType, Component* listener)
{
    // Try to find an empty index.
    std::vector<Component*>& listeners = listenerMap[eventType];
    for (unsigned int i = 0; i < listeners.size(); ++i) {
        // If we found an empty index, add the listener to it.
        if (listeners[i] == nullptr) {
            listeners[i] = listener;
            return;
        }
    }

    // No indices were empty, push to the back of the vector.
    listeners.push_back(listener);
}

void Screen::unregisterListener(InternalEvent::Type eventType, Component* listener)
{
    // Find the given listener.
    std::vector<Component*>& listeners = listenerMap[eventType];
    auto it = std::find(listeners.begin(), listeners.end(), listener);

    // If the listener was found, set it to nullptr.
    // Note: We don't vector.erase() because we don't want to invalidate
    //       iterators.
    if (it != listeners.end()) {
        *it = nullptr;
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
        case SDL_MOUSEWHEEL: {
            return handleMouseWheel(event.wheel);
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

void Screen::tick(double timestepS)
{
    // Call all listener callbacks.
    for (Component* listener : listenerMap[InternalEvent::Tick]) {
        // If the index is empty or the listener isn't visible, continue.
        if ((listener == nullptr)
            || !(listener->getIsVisible())) {
            continue;
        }

        listener->onTick(timestepS);
    }
}

void Screen::render()
{
    // Rendering is left up to derived classes.
}

bool Screen::handleMouseButtonDown(SDL_MouseButtonEvent& event)
{
    bool eventHandled{false};
    for (Component* listener : listenerMap[InternalEvent::MouseButtonDown]) {
        // If the index is empty or the listener isn't visible, continue.
        if ((listener == nullptr)
            || !(listener->getIsVisible())) {
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
    for (Component* listener : listenerMap[InternalEvent::MouseButtonUp]) {
        // If the index is empty or the listener isn't visible, continue.
        if ((listener == nullptr)
            || !(listener->getIsVisible())) {
            continue;
        }

        // Call the listener's callback and track if the event was handled.
        if (listener->onMouseButtonUp(event)) {
            eventHandled = true;
        }
    }

    return eventHandled;
}

bool Screen::handleMouseWheel(SDL_MouseWheelEvent& event)
{
    bool eventHandled{false};
    for (Component* listener : listenerMap[InternalEvent::MouseWheel]) {
        // If the index is empty or the listener isn't visible, continue.
        if ((listener == nullptr)
            || !(listener->getIsVisible())) {
            continue;
        }

        // Call the listener's callback.
        if (listener->onMouseWheel(event)) {
            eventHandled = true;
        }
    }

    return eventHandled;
}

void Screen::handleMouseMove(SDL_MouseMotionEvent& event)
{
    for (Component* listener : listenerMap[InternalEvent::MouseMove]) {
        // If the index is empty or the listener isn't visible, continue.
        if ((listener == nullptr)
            || !(listener->getIsVisible())) {
            continue;
        }

        // Call the listener's callback.
        listener->onMouseMove(event);
    }
}

bool Screen::handleKeyDown(SDL_KeyboardEvent& event)
{
    bool eventHandled{false};
    for (Component* listener : listenerMap[InternalEvent::KeyDown]) {
        // If the index is empty or the listener isn't visible, continue.
        if ((listener == nullptr)
            || !(listener->getIsVisible())) {
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
    for (Component* listener : listenerMap[InternalEvent::TextInput]) {
        // If the index is empty or the listener isn't visible, continue.
        if ((listener == nullptr)
            || !(listener->getIsVisible())) {
            continue;
        }

        // Call the listener's callback and track if the event was handled.
        if (listener->onTextInput(event)) {
            eventHandled = true;
        }
    }

    return eventHandled;
}

} // namespace AUI
