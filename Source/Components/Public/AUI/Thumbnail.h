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
 * Note: This component may be immediately useful to you, but exists more as
 *       an easily copyable example of how to make this sort of thing. With
 *       minor tweaks you can make any sort of thumbnail-based interactable
 *       UI element (e.g. items for an inventory).
 */
class Thumbnail : public Component
{
public:
    //-------------------------------------------------------------------------
    // Public definitions
    //-------------------------------------------------------------------------
    /**
     * Used to track the thumbnail's visual and logical state.
     */
    enum class State {
        Normal,
        Hovered,
        Active
    };

    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    Thumbnail(Screen& screen, const char* key, const SDL_Rect& logicalExtent);

    virtual ~Thumbnail() = default;

    /**
     * Sets the component's state to Active and signals onActivated.
     *
     * Note: This component activates itself when double clicked. This function
     *       just exists in case you need to do something like setting an
     *       initial active component.
     */
    void activate();

    /**
     * Sets the component's state to Normal and signals onDeactivated.
     *
     * Note: This component doesn't deactivate itself. The context that is
     *       managing this component must detect when the component should be
     *       deactivated and call this method.
     */
    void deactivate();

    State getCurrentState();

    /** Background image, normal state. */
    Image normalImage;
    /** Background image, hovered state. */
    Image hoveredImage;
    /** Background image, disabled state. */
    Image activeImage;

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

    std::function<void(Thumbnail*)> onActivated;
    std::function<void(Thumbnail*)> onDeactivated;

    /** Tracks this button's current visual and logical state. */
    State currentState;

    /** Stores the last set horizontal text alignment. We re-apply this
        alignment any time the text is set to a string that fits within its
        extent. */
    Text::HorizontalAlignment savedTextAlignment;
};

} // namespace AUI
