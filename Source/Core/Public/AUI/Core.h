#pragma once

#include "AUI/AssetCache.h"
#include "AUI/ScreenResolution.h"
#include <string>
#include <memory>
#include <atomic>

// Forward declarations.
struct SDL_Renderer;

namespace AUI
{
/**
 * Fulfills two responsibilities:
 *   1. Allows the consumer to configure the library.
 *   2. Maintains common data that library objects need.
 *
 * Note: UI widgets exist in two spaces: logical and actual.
 *       Logical screen space is used for all developer-given widget
 *       measurements, and is relative to the value of Core::logicalScreenSize.
 *       Actual screen space is what's actually used in rendering to the
 *       screen, and is relative to the value of Core::actualScreenSize.
 *
 *       The intent is that the developer will set a logical screen size and
 *       give all width, height, position, etc values in reference to that
 *       logical size. Then, the user can select a new actual size and the UI
 *       will intelligently scale to match it.
 */
class Core
{
public:
    /**
     * Initializes this library (and SDL_image/SDL_ttf if they haven't been).
     * Call this before constructing any widgets.
     *
     * Alternatively, use the Initializer class.
     *
     * @param inSdlRenderer  The renderer to use for constructing textures and
     *                       rendering.
     * @param inLogicalScreenSize  See ScalingHelpers.h class comment.
     */
    static void initialize(SDL_Renderer* inSdlRenderer,
                           ScreenResolution inLogicalScreenSize);

    /**
     * Cleans up this library and SDL_image/SDL_ttf.
     * Don't call unless you're ready to also tear down SDL_image and SDL_ttf.
     *
     * Errors if widgetCount != 0.
     */
    static void quit();

    /**
     * Sets a new screen size for the UI to scale to.
     *
     * Widgets will recognize that this size changed, prompting them to re-
     * calculate their relevant data.
     *
     * See ScalingHelpers.h class comment for full information on AUI scaling.
     */
    static void setActualScreenSize(ScreenResolution inScaledScreenSize);

    /**
     * Sets the distance (in pixels) that the mouse must travel while clicking a
     * draggable widget to trigger a drag and drop event.
     *
     * See EventRouter.h member comments for full information on drag and drop.
     */
    static void setDragTriggerDistance(int newDragTriggerDistance);

    /**
     * If true, a TextInput widget is currently focused and receiving keyboard
     * input.
     *
     * Use this to tell when you should stop polling the keyboard state for
     * held inputs.
     */
    static bool getIsTextInputFocused();

    static SDL_Renderer* getRenderer();
    static ScreenResolution getLogicalScreenSize();
    static ScreenResolution getActualScreenSize();
    static AUI::AssetCache& getAssetCache();
    static int getSquaredDragTriggerDistance();

private:
    /** Friend Widget so it can update the widget count. */
    friend class Widget;

    /** Friend TextInput so it can update isTextInputFocused. */
    friend class TextInput;

    /**
     * Increases the count of currently constructed widgets.
     */
    static void incWidgetCount();
    /**
     * Decreases the count of currently constructed widgets.
     */
    static void decWidgetCount();

    /** The renderer to use when constructing textures and rendering. */
    static SDL_Renderer* sdlRenderer;

    /** See ScalingHelpers.h class comment. */
    static ScreenResolution logicalScreenSize;

    /** See ScalingHelpers.h class comment. */
    static ScreenResolution actualScreenSize;

    /** The asset cache for font objects. */
    static std::unique_ptr<AssetCache> assetCache;

    /** The distance in pixels that the mouse must travel to trigger to a drag
        and drop event. */
    static int dragTriggerDistance;
    /** The squared dragTriggerDistance, for more efficient calculations. */
    static int squaredDragTriggerDistance;

    /** Keeps a count of the number of currently constructed widgets.
        Used to check if it's safe to Quit(). */
    static std::atomic<int> widgetCount;

    /** See getIsTextInputFocused(). */
    static std::atomic<bool> isTextInputFocused;
};

} // namespace AUI
