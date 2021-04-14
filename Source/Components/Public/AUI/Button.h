#pragma once

#include "AUI/Image.h"
#include "AUI/Text.h"

namespace AUI {

/**
 * Displays a simple static image.
 */
class Button : public Component
{
public:
    Button(Screen& screen, const char* key, const SDL_Rect& screenExtent);

    virtual ~Button() = default;

    void render(int offsetX = 0, int offsetY = 0) override;

    //-------------------------------------------------------------------------
    // Public child components
    //-------------------------------------------------------------------------
    /** Background image, normal state. */
    Image normalImage;
    /** Background image, hovered state. */
    Image hoveredImage;
    /** Background image, pressed state. */
    Image pressedImage;
    /** Background image, disabled state. */
    Image disabledImage;

    /** Button text. */
    Text text;
};

} // namespace AUI
