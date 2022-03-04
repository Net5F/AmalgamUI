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
    /** If true, this event was consumed and should not propagate any
        further. */
    bool wasConsumed{false};

    /** If non-nullptr, mouse capture should be set to the provided widget. */
    Widget* setMouseCapture{nullptr};

    /** If true, mouse capture should be released. */
    bool releaseMouseCapture{false};
};

} // namespace AUI
