#pragma once

#include "AUI/Component.h"
#include <vector>
#include <memory>

namespace AUI {

/**
 * Base class for container components.
 */
template <typename T>
class Container : public Component
{
public:
    Container(Screen& screen, const char* key, const SDL_Rect& screenExtent)
    : Component(screen, key, screenExtent)
    {
    }

    virtual ~Container() = default;

    void add(std::unique_ptr<T> newElement)
    {
        elements.push_back(std::move(newElement));
    }

protected:
    std::vector<std::unique_ptr<T>> elements;
};

} // namespace AUI
