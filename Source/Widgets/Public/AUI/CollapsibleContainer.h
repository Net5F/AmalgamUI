#pragma once

#include "AUI/Container.h"
#include "AUI/Image.h"
#include "AUI/Text.h"

namespace AUI
{
/**
 * Initially, this container shows only a header with an image and some text.
 * When clicked, it expands to reveal its child elements. The header can then
 * be clicked to collapse it again.
 *
 * By default, the whole header acts as the click region for the expand/collapse
 * action. You can modify this using setClickRegionLogicalExtent().
 *
 * Note: The extent that you give this widget to will be used as the header
 *       size. When the container is expanded, it will get taller to fit its
 *       elements.
 */
class CollapsibleContainer : public Container
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    CollapsibleContainer(const SDL_FRect& inLogicalExtent,
                         const std::string& inDebugName
                         = "CollapsibleContainer");

    virtual ~CollapsibleContainer() = default;

    /**
     * Sets the extent of this widget's expand/collapse click region.
     */
    void setClickRegionLogicalExtent(const SDL_FRect& inLogicalExtent);

    /**
     * Sets whether the container is collapsed or expanded.
     */
    void setIsCollapsed(bool inIsCollapsed);

    /**
     * Sets the distance between elements.
     */
    void setGapSize(float inLogicalGapSize);

    /**
     * @return The clipped, window-relative extent for this widget's header.
     */
    SDL_FRect getHeaderExtent();

    /**
     * @return The clipped, window-relative extent for this widget's click
     *         region.
     */
    SDL_FRect getClickRegionExtent();

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
    /** The image shown in the header if this container is expanded. */
    Image expandedImage;

    /** The image shown in the header if this container is collapsed. */
    Image collapsedImage;

    /** The header text, to the right of the arrow. */
    Text headerText;

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void setLogicalExtent(const SDL_FRect& inLogicalExtent) override;

    EventResult onMouseDown(MouseButtonType buttonType,
                            const SDL_FPoint& cursorPosition) override;

    EventResult onMouseDoubleClick(MouseButtonType buttonType,
                                   const SDL_FPoint& cursorPosition) override;

    void measure(const SDL_FRect& availableExtent) override;

    void arrange(const SDL_FPoint& startPosition,
                 const SDL_FRect& availableExtent,
                 WidgetLocator* widgetLocator) override;

protected:
    /**
     * Measures the height of this widget, if it was in an expanded state.
     * Sets logicalExtent.h and scaledExtent.h to fit our elements.
     */
    void measureExpandedHeight();

    /** The logical extent of the header. Used to return to the original size
        when this container goes from expanded to collapsed. */
    SDL_FRect headerLogicalExtent;

    /** The region that must be clicked to expand or collapse this widget. */
    SDL_FRect clickRegionLogicalExtent;

    /** If true, the container is collapsed. If false, it's expanded. */
    bool isCollapsed;

    /** The size in logical space of the gap between elements. */
    float logicalGapSize;
    /** The scaled size in actual space of the gap between elements. */
    float scaledGapSize;
};

} // namespace AUI
