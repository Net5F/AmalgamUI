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
    /** The mouse clicked on the component. */
    MouseButtonDown,
    /** The mouse released a previous click. */
    MouseButtonUp,
    /** The mouse moved within the component's extent. */
    MouseMove,
    /** The mouse entered the component's extent. */
    MouseEnter,
    /** The mouse left the component's extent. */
    MouseLeave
};

} // namespace AUI
