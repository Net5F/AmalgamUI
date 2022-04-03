#pragma once

namespace AUI
{
/**
 * Refers to the cause of a FocusLost event.
 */
enum class FocusLostType
{
    /** The Escape key was pressed. */
    Escape,
    /** A click occurred outside of the focused widget. */
    Click,
    /** Focus was explicitly requested to be dropped. */
    Requested,
    /** Focus was set to a new widget. */
    NewFocus
};

} // namespace AUI
