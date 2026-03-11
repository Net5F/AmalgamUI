#pragma once

#include "AUI/Container.h"
#include "AUI/Orientation.h"

namespace AUI
{
/**
 * Lays out widgets in a scrollable vertical list.
 *
 * Supports vertical scrolling with the mouse wheel.
 *
 * TODO: After scrolling, our elements still have hover states based on their
 *       pre-scroll positions. If possible, finding a way to update them would
 *       be nice.
 */
class ScrollArea : public Widget
{
public:
    //-------------------------------------------------------------------------
    // Public definitions
    //-------------------------------------------------------------------------
    /**
     * Defines the origin point used when calculating scroll behavior.
     */
    enum class ScrollOrigin {
        /** Default behavior. Starts at the top and scrolls downwards. */
        TopLeft,
        /** Starts at the bottom and scrolls upwards. Used for things like
            chat boxes. */
        BottomLeft
    };

    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    ScrollArea(const SDL_FRect& inLogicalExtent,
               const std::string& inDebugName = "ScrollArea");

    virtual ~ScrollArea() = default;

    /**
     * Sets how far a single horizontal scroll event should scroll the
     * container.
     */
    void setScrollStepX(float inLogicalScrollStepX);

    /**
     * Sets how far a single vertical scroll event should scroll the container.
     */
    void setScrollStepY(float inLogicalScrollStepY);

    /**
     * Sets which direction the mouse wheel will scroll in.
     */
    void setScrollOrientation(Orientation inScrollOrientation);

    /**
     * Defines the origin point used when calculating scroll behavior.
     *
     * Change this to BottomLeft to make a chat box.
     */
    void setScrollOrigin(ScrollOrigin inScrollOrigin);

    /**
     * @return How far we're scrolled in the relevant direction, in scaled
     *         units.
     */
    float getScrollDistanceX();
    float getScrollDistanceY();

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
    /** This scroll area's child content widget.
        We use this instead of adding it to Widget::children so that we can
        control its layout independent of our own. */
    std::unique_ptr<Widget> content;

    //-------------------------------------------------------------------------
    // Widget class overrides
    //-------------------------------------------------------------------------
    EventResult onMouseWheel(float amountScrolled) override;

    void onTick(double timestepS) override;

    void measure(const SDL_FRect& availableExtent) override;

    void arrange(const SDL_FPoint& startPosition,
                 const SDL_FRect& availableExtent,
                 WidgetLocator* widgetLocator) override;

    void render(const SDL_FPoint& windowTopLeft) override;

private:
    void handleMouseScrollHorizontal(float amountScrolled);
    void handleMouseScrollVertical(float amountScrolled);

    /**
     * Calculates the actual-space extent of our child's content.
     */
    SDL_FRect calcContentExtent() const;

    /** The default logical pixel distance of a horizontal scroll event. */
    static constexpr float LOGICAL_DEFAULT_SCROLL_STEP_X{15};

    /** The default logical pixel distance of a vertical scroll event. */
    static constexpr float LOGICAL_DEFAULT_SCROLL_STEP_Y{15};

    /** The width in logical space of a single scroll event. */
    float logicalScrollStepX;
    /** The scaled width in actual space of a single scroll event. */
    float scaledScrollStepX;

    /** The height in logical space of a single scroll event. */
    float logicalScrollStepY;
    /** The scaled height in actual space of a single scroll event. */
    float scaledScrollStepY;

    /** The direction that the mouse wheel will scroll in. */
    Orientation scrollOrientation;

    /** Defines the origin point used when calculating scroll behavior. */
    ScrollOrigin scrollOrigin;

    /** How far we're currently scrolled along the X axis, in scaled units. */
    float scrollDistanceX;

    /** How far we're currently scrolled along the Y axis, in scaled units. */
    float scrollDistanceY;
};

} // namespace AUI
