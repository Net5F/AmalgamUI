#pragma once

#include "AUI/Component.h"
#include "AUI/Internal/Log.h"
#include <vector>
#include <memory>

namespace AUI {

/**
 * Base class for container components.
 */
class Container : public Component
{
public:
    Container(Screen& screen, const char* key, const SDL_Rect& logicalExtent)
    : Component(screen, key, logicalExtent)
    {
    }

    virtual ~Container() = default;

    void push_back(std::unique_ptr<Component> newElement)
    {
        elements.push_back(std::move(newElement));
    }

    void erase(unsigned int index) {
        if (elements.size() <= index) {
            AUI_LOG_ERROR("Tried to remove element that doesn't exist in "
            "container. Index: %u, Size: %u", index, elements.size());
        }

        elements.erase(elements.begin());
    }

protected:
    std::vector<std::unique_ptr<Component>> elements;
};

} // namespace AUI
