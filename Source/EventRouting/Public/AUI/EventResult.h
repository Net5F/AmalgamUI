#pragma once

namespace AUI
{

class Widget;

/**
 * The result of a call to a Widget's event handler.
 *
 * Gives the widget a chance to request that certain actions be performed by
 * the EventRouter.
 *
 * Typically, we will return this using designated initializers:
 *     return EventResult{.eventWasConsumed{true}, .setMouseCapture{this}};
 */
struct EventResult {
public:
    /** If true, this event was handled and should not propagate any
        further. */
    bool wasHandled{false};

    /** If non-nullptr, mouse capture should be set to the given widget. */
    Widget* setMouseCapture{nullptr};

    /** If true, mouse capture should be released. */
    bool releaseMouseCapture{false};

    /** If non-nullptr, focus should be set to the given widget.
        If focus is already set to a different widget, it will be dropped
        and set to the given widget. */
    Widget* setFocus{nullptr};

    /** If true, focus should be dropped. */
    bool dropFocus{false};
};

} // namespace AUI
