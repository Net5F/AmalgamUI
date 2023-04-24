#pragma once

#include "AUI/Container.h"

namespace AUI
{
/**
 * Lays out widgets in a scrollable vertical list.
 *
 * TODO: After scrolling the list, our elements still have hover states based
 *       on their pre-scroll positions. We need to find a way to update them.
 */
class VerticalListContainer : public Container
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    VerticalListContainer(const SDL_Rect& inLogicalExtent,
                  const std::string& inDebugName = "VerticalListContainer");

    virtual ~VerticalListContainer() = default;

    /**
     * Sets the distance between elements.
     */
    void setGapSize(int inLogicalGapSize);

    /**
     * Sets how far a single scroll event should scroll the container.
     */
    void setScrollHeight(int inLogicalScrollHeight);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    EventResult onMouseWheel(int amountScrolled) override;

    void updateLayout(const SDL_Point& startPosition,
                      const SDL_Rect& availableExtent,
                      WidgetLocator* widgetLocator) override;

private:
    /**
     * Calculates the height of this container's content, including gaps.
     */
    int calcContentHeight();

    /** The default logical pixel distance of a scroll event. */
    static constexpr int LOGICAL_DEFAULT_SCROLL_DISTANCE{15};

    /** The height in logical space of a single scroll event. */
    int logicalScrollHeight;
    /** The scaled height in actual space of a single scroll event. */
    int scaledScrollHeight;

    /** The size in logical space of the gap between elements. */
    int logicalGapSize;
    /** The scaled size in actual space of the gap between elements. */
    int scaledGapSize;

    /** How far we're currently scrolled.
        Note that this should be subtracted from element Y values, since
        scrolling down makes the elements go up. */
    int scrollDistance;
};

} // namespace AUI
