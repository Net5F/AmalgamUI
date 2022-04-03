#pragma once

#include "AUI/Image.h"
#include "AUI/Text.h"
#include <functional>

namespace AUI
{
/**
 * An interactable element with a thumbnail image and text.
 *
 * Has a double-click Active state.
 *
 * The rendering order for this widget's children is:
 *   Background: activeImage, hoveredImage
 *   Middle-ground: backdropImage, selectedImage
 *   Foreground: thumbnailImage, text
 *
 * Note: This widget may be immediately useful to you, but exists more as
 *       an easily copyable example of how to make this sort of thing. With
 *       minor tweaks you can make any sort of thumbnail-based interactable
 *       UI element (e.g. items for an inventory).
 */
class Thumbnail : public Widget
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    Thumbnail(const SDL_Rect& inLogicalExtent,
              const std::string& inDebugName = "Thumbnail");

    virtual ~Thumbnail() = default;

    /**
     * Selects this widget and calls onSelected.
     *
     * If this widget is already selected or isSelectable == false, does
     * nothing.
     *
     * Note: This widget selects itself when clicked. This function just
     *       exists in case you need to do it programatically.
     */
    void select();

    /**
     * Deselects this widget and calls onDeselected.
     *
     * If this widget isn't selected, does nothing.
     *
     * Note: This widget doesn't deselect itself. The context that is
     *       managing this widget must detect when the widget should be
     *       deactivated and call this method.
     */
    void deselect();

    /**
     * Activates this widget and calls onActivated.
     *
     * If this widget is already active or isActivateable == false, does
     * nothing.
     *
     * Disables hovering. Any active hover state will be removed.
     *
     * Note: This widget activates itself when double clicked. This function
     *       just exists in case you need to do it programatically.
     */
    void activate();

    /**
     * Deactivates this widget and calls onDeactivated.
     *
     * If this widget isn't active, does nothing.
     *
     * Note: This widget doesn't deactivate itself. The context that is
     *       managing this widget must detect when the widget should be
     *       deactivated and call this method.
     */
    void deactivate();

    bool getIsHovered();
    bool getIsSelected();
    bool getIsActive();

    /** If true, this widget is able to be hovered. */
    void setIsHoverable(bool inIsHoverable);

    /** If true, this widget is able to be selected. */
    void setIsSelectable(bool inIsSelectable);

    /** If true, this widget is able to be activated. */
    void setIsActivateable(bool inIsActivateable);

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
    /** Background image, hovered state. */
    Image hoveredImage;
    /** Background image, active state. */
    Image activeImage;

    /** Middle-ground backdrop image, always visible. */
    Image backdropImage;
    /** Middle-ground image, selected state. */
    Image selectedImage;

    /** Foreground thumbnail image. */
    Image thumbnailImage;

    //-------------------------------------------------------------------------
    // Limited public interface of private widgets
    //-------------------------------------------------------------------------
    /**
     * Calls text.setText().
     * When the text gets too large and starts clipping its bounds, we right-
     * align it to show the user the most relevant info.
     */
    void setText(std::string_view inText);

    /**
     * Calls text.setHorizontalAlignment().
     * When the text gets too large and starts clipping its bounds, we right-
     * align it to show the user the most relevant info.
     */
    void setTextHorizontalAlignment(
        Text::HorizontalAlignment inHorizontalAlignment);

    /** Calls text.setLogicalExtent(). */
    void setTextLogicalExtent(const SDL_Rect& inLogicalExtent);
    /** Calls text.setFont(). */
    void setTextFont(const std::string& relPath, int size);
    /** Calls text.setColor(). */
    void setTextColor(const SDL_Color& inColor);
    /** Calls text.setVerticalAlignment(). */
    void setTextVerticalAlignment(Text::VerticalAlignment inVerticalAlignment);

    //-------------------------------------------------------------------------
    // Callback registration
    //-------------------------------------------------------------------------
    /**
     * @param inOnSelected  A callback that expects a pointer to the widget
     *                      that was selected.
     */
    void setOnSelected(std::function<void(Thumbnail*)> inOnSelected);

    /**
     * @param inOnDeselected  A callback that expects a pointer to the
     *                        widget that was deselected.
     */
    void setOnDeselected(std::function<void(Thumbnail*)> inOnDeselected);

    /**
     * @param inOnActivated  A callback that expects a pointer to the widget
     *                       that was activated.
     */
    void setOnActivated(std::function<void(Thumbnail*)> inOnActivated);

    /**
     * @param inOnDeactivated  A callback that expects a pointer to the
     *                         widget that was deactivated.
     */
    void setOnDeactivated(std::function<void(Thumbnail*)> inOnDeactivated);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    EventResult onMouseDown(MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;

    EventResult onMouseDoubleClick(MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition) override;

    void onMouseEnter() override;

    void onMouseLeave() override;

private:
    /** Sets isHovered and updates the visibility of hoveredImage. */
    void setIsHovered(bool inIsHovered);
    /** Sets isSelected and updates the visibility of selectedImage. */
    void setIsSelected(bool inIsSelected);
    /** Sets isActive and updates the visibility of activeImage. */
    void setIsActive(bool inIsActive);

    /**
     * Uses the given actual-space mouse position to check if this widget
     * should be hovered or unhovered.
     */
    Widget* updateHovered(SDL_Point actualMousePoint);

    std::function<void(Thumbnail*)> onSelected;
    std::function<void(Thumbnail*)> onDeselected;
    std::function<void(Thumbnail*)> onActivated;
    std::function<void(Thumbnail*)> onDeactivated;

    /** If true, this widget is able to be hovered. */
    bool isHoverable;

    /** If true, this widget is able to be selected. */
    bool isSelectable;

    /** If true, this widget is able to be activated. */
    bool isActivateable;

    /** Tracks whether the mouse is currently hovering over this widget. */
    bool isHovered;

    /** Tracks whether this widget is currently selected. */
    bool isSelected;

    /** Tracks whether this widget is currently active. */
    bool isActive;

    /** Stores the last set horizontal text alignment. We re-apply this
        alignment any time the text is set to a string that fits within its
        extent. */
    Text::HorizontalAlignment savedTextAlignment;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    /** The text that goes under the thumbnail image. Private since we need to
        right-align the text if it gets too large. */
    Text text;
};

} // namespace AUI
