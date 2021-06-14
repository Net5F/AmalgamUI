#pragma once

#include "AUI/Image.h"
#include "AUI/Text.h"
#include <functional>

namespace AUI {

/**
 * An interactable element with a thumbnail image and text.
 *
 * Has a double-click Active state.
 *
 * The rendering order for this component's children is:
 *   Background: activeImage, hoveredImage
 *   Middle-ground: backdropImage, selectedImage
 *   Foreground: thumbnailImage, text
 *
 * Note: This component may be immediately useful to you, but exists more as
 *       an easily copyable example of how to make this sort of thing. With
 *       minor tweaks you can make any sort of thumbnail-based interactable
 *       UI element (e.g. items for an inventory).
 */
class Thumbnail : public Component
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    Thumbnail(Screen& screen, const char* key, const SDL_Rect& logicalExtent);

    virtual ~Thumbnail() = default;

    /**
     * Selects this component and calls onSelected.
     *
     * If this component is already selected or isSelectable == false, does
     * nothing.
     *
     * Note: This component selects itself when clicked. This function just
     *       exists in case you need to do it programatically.
     */
    void select();

    /**
     * Deselects this component and calls onDeselected.
     *
     * If this component isn't selected, does nothing.
     *
     * Note: This component doesn't deselect itself. The context that is
     *       managing this component must detect when the component should be
     *       deactivated and call this method.
     */
    void deselect();

    /**
     * Activates this component and calls onActivated.
     *
     * If this component is already active or isActivateable == false, does
     * nothing.
     *
     * Disables hovering. Any active hover state will be removed.
     *
     * Note: This component activates itself when double clicked. This function
     *       just exists in case you need to do it programatically.
     */
    void activate();

    /**
     * Deactivates this component and calls onDeactivated.
     *
     * If this component isn't active, does nothing.
     *
     * Note: This component doesn't deactivate itself. The context that is
     *       managing this component must detect when the component should be
     *       deactivated and call this method.
     */
    void deactivate();

    /** If true, this component is able to be hovered. */
    void setIsHoverable(bool inIsHoverable);

    /** If true, this component is able to be selected. */
    void setIsSelectable(bool inIsSelectable);

    /** If true, this component is able to be activated. */
    void setIsActivateable(bool inIsActivateable);

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
    // Limited public interface of private components
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
    void setTextHorizontalAlignment(Text::HorizontalAlignment inHorizontalAlignment);

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
     * @param inOnSelected  A callable that expects a pointer to the component
     *                      that was selected.
     */
    void setOnSelected(std::function<void(Thumbnail*)> inOnSelected);

    /**
     * @param inOnDeselected  A callable that expects a pointer to the
     *                        component that was deselected.
     */
    void setOnDeselected(std::function<void(Thumbnail*)> inOnDeselected);

    /**
     * @param inOnActivated  A callable that expects a pointer to the component
     *                       that was activated.
     */
    void setOnActivated(std::function<void(Thumbnail*)> inOnActivated);

    /**
     * @param inOnDeactivated  A callable that expects a pointer to the
     *                         component that was deactivated.
     */
    void setOnDeactivated(std::function<void(Thumbnail*)> inOnDeactivated);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    bool onMouseButtonDown(SDL_MouseButtonEvent& event) override;

    void onMouseMove(SDL_MouseMotionEvent& event) override;

    void render(const SDL_Point& parentOffset = {}) override;

private:
    //-------------------------------------------------------------------------
    // Private members
    //-------------------------------------------------------------------------
    /** The text that goes under the thumbnail image. Private since we need to
        right-align the text if it gets too large. */
    Text text;

    std::function<void(Thumbnail*)> onSelected;
    std::function<void(Thumbnail*)> onDeselected;
    std::function<void(Thumbnail*)> onActivated;
    std::function<void(Thumbnail*)> onDeactivated;

    /** If true, this component is able to be hovered. */
    bool isHoverable;

    /** If true, this component is able to be selected. */
    bool isSelectable;

    /** If true, this component is able to be activated. */
    bool isActivateable;

    /** Tracks whether the mouse is currently hovering over this component. */
    bool isHovered;

    /** Tracks whether this component is currently selected. */
    bool isSelected;

    /** Tracks whether this component is currently active. */
    bool isActive;

    /** Stores the last set horizontal text alignment. We re-apply this
        alignment any time the text is set to a string that fits within its
        extent. */
    Text::HorizontalAlignment savedTextAlignment;
};

} // namespace AUI
