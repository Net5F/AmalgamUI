#include "AUI/Window.h"
#include "AUI/Internal/Log.h"

namespace AUI
{
Window::Window(Screen& inScreen, const SDL_Rect& inLogicalExtent,
                     const std::string& inDebugName)
: Widget(inScreen, inLogicalExtent, inDebugName)
, lastHoveredChild{nullptr}
, lastClickedChild{nullptr}
{
}

Widget* Window::handleOSEvent(SDL_Event& event)
{
    // Propagate the event through our children.
    Widget* consumer{passOSEventToChildren(event)};

    // None of our children handled the event. Try to handle it ourselves.
    if (consumer == nullptr) {
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN: {
                return onMouseButtonDown(event.button);
            }
            case SDL_MOUSEBUTTONUP: {
                return onMouseButtonUp(event.button);
            }
            case SDL_MOUSEMOTION: {
                return onMouseMove(event.motion);
            }
            case SDL_MOUSEWHEEL: {
                return onMouseWheel(event.wheel);
            }
            case SDL_KEYDOWN: {
                return onKeyDown(event.key);
            }
            case SDL_TEXTINPUT: {
                return onTextInput(event.text);
            }
            default:
                break;
        }
    }

    return consumer;
}

Widget* Window::passOSEventToChildren(SDL_Event& event)
{
    // Propagate the event through our visible child widgets.
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        // If the child isn't visible, skip it.
        Widget& child{it->get()};
        if (!(child.getIsVisible())) {
            continue;
        }

        // If the child consumed this event, update our tracking and return
        // true.
        Widget* consumer{child.handleOSEvent(event)};
        if (consumer != nullptr) {
            if (event.type == SDL_MOUSEMOTION) {
                // If a new widget was hovered, send the MouseMove to the last
                // hovered widget so it can unhover itself.
                if ((lastHoveredChild != nullptr)
                    && (consumer != lastHoveredChild)) {
                    lastHoveredChild->handleOSEvent(event);
                }

                // Track the hovered widget.
                lastHoveredChild = consumer;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                // If a new widget was clicked, send the MouseDown to the last
                // clicked widget so it can deselect itself.
                if ((lastClickedChild != nullptr)
                    && (consumer != lastClickedChild)) {
                    lastClickedChild->handleOSEvent(event);
                }

                // Track the clicked widget.
                lastClickedChild = consumer;
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {
                // If the mouse wasn't released over our last clicked widget,
                // send it the MouseUp event so it can resolve its state.
                if ((lastClickedChild != nullptr)
                    && consumer != lastClickedChild) {
                    lastClickedChild->handleOSEvent(event);
                }
            }

            return consumer;
        }
    }

    // If the mouse moved outside of the hovered widget, clear it.
    if ((event.type == SDL_MOUSEMOTION)
        && (lastHoveredChild != nullptr)) {
        lastHoveredChild->handleOSEvent(event);
        lastHoveredChild = nullptr;
    }
    // If the mouse clicked outside of the clicked widget, clear it.
    else if ((event.type == SDL_MOUSEBUTTONDOWN)
        && (lastClickedChild != nullptr)) {
        lastClickedChild->handleOSEvent(event);
        lastClickedChild = nullptr;
    }

    return nullptr;
}

void Window::tick(double timestepS)
{
    // TODO: Fix this to call every child for real
    // Call every child's tick.
    for (Widget& child : children) {
        // If the child isn't visible, skip it.
        if (!(child.getIsVisible())) {
            continue;
        }

        child.onTick(timestepS);
    }
}

} // namespace AUI
