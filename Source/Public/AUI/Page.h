#pragma once

#include "Component.h"
#include <vector>

namespace AUI {

/**
 * This class represents a UI page.
 *
 * Pages facilitate the organization of UI components, and provide an easy way
 * to switch between sets of components.
 *
 * An appropriate page may be a title page, settings page, or a world page that
 * provides UI elements while allowing the user to see the world behind it.
 */
class Page
{
public:
    void addComponent(Component& component);

    std::vector<Component> components;
};

} // namespace AUI
