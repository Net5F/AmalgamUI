#pragma once

#include "AUI/Container.h"
#include <vector>
#include <memory>

namespace AUI {

/**
 * Lays out components in a vertical grid.
 */
class VerticalGridContainer : public Container
{
public:
    // Bring base class members into current namespace.
    using Component::scaledExtent;
    using Component::isVisible;
    using Container::elements;

    VerticalGridContainer(Screen& screen, const char* key, const SDL_Rect& logicalExtent)
    : Container(screen, key, logicalExtent)
    {
    }

    virtual ~VerticalGridContainer() = default;

    void render(const SDL_Point& parentOffset = {}) override
    {
        // If the component isn't visible, return without rendering.
        if (!isVisible) {
            return;
        }

        // Add our position to the given offset.
        int offsetX{parentOffset.x};
        int offsetY{parentOffset.y};
        offsetX += scaledExtent.x;
        offsetY += scaledExtent.y;

        for (std::unique_ptr<Component>& element : elements) {
            element->render({offsetX, offsetY});
            offsetY += 100;
        }
    }
};

} // namespace AUI
