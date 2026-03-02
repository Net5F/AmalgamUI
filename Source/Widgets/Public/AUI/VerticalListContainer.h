#pragma once

#include "AUI/Container.h"

namespace AUI
{
/**
 * Lays out widgets in a scrollable vertical list.
 *
 * Supports vertical scrolling with the mouse wheel.
 *
 * TODO: After scrolling the list, our elements still have hover states based
 *       on their pre-scroll positions. We need to find a way to update them.
 * TODO: Remake this using ScrollArea, replacing FlowDirection with ScrollOrigin
 *       for chat box behavior.
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
    VerticalListContainer(const SDL_FRect& inLogicalExtent,
                          const std::string& inDebugName
                          = "VerticalListContainer");

    virtual ~VerticalListContainer() = default;

    /**
     * Sets the distance between elements.
     */
    void setGapSize(float inLogicalGapSize);

    /**
     * Sets how far a single scroll event should scroll the container.
     */
    void setScrollHeight(float inLogicalScrollHeight);

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
    EventResult onMouseWheel(float amountScrolled) override;

    void measure(const SDL_FRect& availableExtent) override;

    void arrange(const SDL_FPoint& startPosition,
                 const SDL_FRect& availableExtent,
                 WidgetLocator* widgetLocator) override;

private:
    /**
     * Calculates the height of this container's content, including gaps.
     */
    float calcContentHeight();

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
    static constexpr float LOGICAL_DEFAULT_SCROLL_DISTANCE{15};

    /** The height in logical space of a single scroll event. */
    float logicalScrollHeight;
    /** The scaled height in actual space of a single scroll event. */
    float scaledScrollHeight;

    /** The size in logical space of the gap between elements. */
    float logicalGapSize;
    /** The scaled size in actual space of the gap between elements. */
    float scaledGapSize;

    /** The direction that child widgets should flow in. See
        setFlowDirection(). */
    FlowDirection flowDirection;

    /** How far we're currently scrolled, in scaled units. */
    float scrollDistance;
};

} // namespace AUI
