#pragma once

#include "AUI/Container.h"
#include "AUI/Image.h"
#include "AUI/Text.h"
#include <functional>

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
    CollapsibleContainer(const SDL_Rect& inLogicalExtent,
                  const std::string& inDebugName = "CollapsibleContainer");

    virtual ~CollapsibleContainer() = default;

    /**
     * Sets the extent of this widget's expand/collapse click region.
     */
    void setClickRegionLogicalExtent(const SDL_Rect& inLogicalExtent);

    /**
     * Sets whether the container is collapsed or expanded.
     */
    void setIsCollapsed(bool inIsCollapsed);

    /**
     * Sets the distance between elements.
     */
    void setGapSize(int inLogicalGapSize);

    /**
     * @return The clipped, window-relative extent for this widget's header.
     */
    SDL_Rect getHeaderExtent();

    /**
     * @return The clipped, window-relative extent for this widget's click 
     *         region.
     */
    SDL_Rect getClickRegionExtent();

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
    // Callback registration
    //-------------------------------------------------------------------------
    /**
     * @param inOnHeightChanged  A callback for when this container is expanded
     *                           or collapsed.
     */
    void setOnHeightChanged(std::function<void(void)> inOnHeightChanged);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void setLogicalExtent(const SDL_Rect& inLogicalExtent) override;

    EventResult onMouseDown(MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;

    EventResult onMouseDoubleClick(MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition) override;

    void updateLayout(const SDL_Point& startPosition,
                      const SDL_Rect& availableExtent,
                      WidgetLocator* widgetLocator) override;

protected:
    /**
     * Calculates the height of this widget, if it was in an expanded state.
     */
    int calcExpandedHeight();

    std::function<void(void)> onHeightChanged;

    /** The logical extent of the header. Used to return to the original size 
        when this container goes from expanded to collapsed. */
    SDL_Rect headerLogicalExtent;

    /** The region that must be clicked to expand or collapse this widget. */
    SDL_Rect clickRegionLogicalExtent;

    /** If true, the container is collapsed. If false, it's expanded. */
    bool isCollapsed;

    /** The size in logical space of the gap between elements. */
    int logicalGapSize;
    /** The scaled size in actual space of the gap between elements. */
    int scaledGapSize;
};

} // namespace AUI
