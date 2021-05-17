#pragma once

#include "AUI/Container.h"
#include <vector>
#include <memory>

namespace AUI {

/**
 * Lays out components in a vertical grid.
 */
template <typename T>
class VerticalGridContainer : public Container<T>
{
public:
    // Bring base class members into current namespace.
    using Component::actualScreenExtent;
    using Container<T>::elements;

    VerticalGridContainer(Screen& screen, const char* key, const SDL_Rect& screenExtent)
    : Container<T>(screen, key, screenExtent)
    {
    }

    virtual ~VerticalGridContainer() = default;

    void render(const SDL_Point& offsetPoint = {}) override
    {
        // Add our position to the given offset.
        int offsetX{offsetPoint.x};
        int offsetY{offsetPoint.y};
        offsetX += actualScreenExtent.x;
        offsetY += actualScreenExtent.y;

        for (std::unique_ptr<T>& element : elements) {
            element->render({offsetX, offsetY});
            offsetY += 50;
        }
    }
};

} // namespace AUI
