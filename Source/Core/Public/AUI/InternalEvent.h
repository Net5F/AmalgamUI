#pragma once

namespace AUI
{
/**
 * The types of internal events that our UI components can register to receive.
 *
 * Note: Internal events are named to match their corresponding SDL2 events.
 */
struct InternalEvent {
    enum Type : unsigned int {
        /** The mouse was clicked within the window. */
        MouseButtonDown = 0,
        /** The mouse released a click within the window. */
        MouseButtonUp = 1,
        /** The mouse wheel was scrolled within the window. */
        MouseWheel = 2,
        /** The mouse moved within the window. */
        MouseMove = 3,
        /** A key was pressed while the window was selected. */
        KeyDown = 4,
        /** Text input is enabled and a key or combination of keys was pressed,
            resulting in a unicode text character. */
        TextInput = 5,
        /** The tick timestep has passed. */
        Tick = 6
    };

    /** The number of event types. */
    static constexpr unsigned int NUM_TYPES = Type::Tick + 1;
};

} // namespace AUI
