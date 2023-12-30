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
    // Public definitions
    //-------------------------------------------------------------------------
    /**
     * The direction that this container's elements should flow in. See
     * setDirection().
     */
    enum class FlowDirection { TopToBottom, BottomToTop };

    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    VerticalListContainer(const SDL_Rect& inLogicalExtent,
                          const std::string& inDebugName
                          = "VerticalListContainer");

    virtual ~VerticalListContainer() = default;

    /**
     * Sets the distance between elements.
     */
    void setGapSize(int inLogicalGapSize);

    /**
     * Sets how far a single scroll event should scroll the container.
     */
    void setScrollHeight(int inLogicalScrollHeight);

    /**
     * Sets whether widgets get added to the top of this container's extent and
     * flow downwards, or to the bottom of its extent and flow upwards.
     *
     * TopToBottom is good for a normal list, BottomToTop is good for a chat
     * box.
     */
    void setFlowDirection(FlowDirection inFlowDirection);

    //-------------------------------------------------------------------------
    // Widget class overrides
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

    /**
     * Lay out our elements starting at the top of this container's extent and
     * growing downwards
     */
    void arrangeElementsTopToBottom(WidgetLocator* widgetLocator);

    /**
     * Lay out our elements starting at the bottom of this container's extent
     * and growing upwards.
     */
    void arrangeElementsBottomToTop(WidgetLocator* widgetLocator);

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

    /** The direction that child widgets should flow in. See
        setFlowDirection(). */
    FlowDirection flowDirection;

    /** How far we're currently scrolled, in scaled units. */
    int scrollDistance;
};

} // namespace AUI
