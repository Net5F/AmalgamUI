#pragma once

namespace AUI {

/**
 * The types of internal events that our UI components can register to receive.
 *
 * Note that internal events are named differently to differentiate them from
 * the naming convention of user-facing external events (e.g. MouseButtonDown
 * vs Pressed).
 */
enum EventType {
    /** The mouse clicked within the window. */
    MouseButtonDown,
    /** The mouse released a click within the window. */
    MouseButtonUp,
    /** The mouse moved within the window. */
    MouseMove,
    /** A key was pressed while the window was selected. */
    KeyDown,
    /** Text input is enabled and a key or combination of keys was pressed,
        resulting in a unicode text character. */
    TextInput,
    /** The tick timestep has passed. */
    Tick
};

} // namespace AUI
