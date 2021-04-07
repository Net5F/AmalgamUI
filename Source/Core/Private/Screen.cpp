#include "AUI/Screen.h"

namespace AUI {

Screen::Screen(const std::string& inDebugName)
: debugName{inDebugName}
, components()
, componentMap()
{
}

void Screen::remove(entt::hashed_string key)
{
    // If we don't have a component with that key, fail.
    auto mapIt = componentMap.find(key);
    if (mapIt == componentMap.end()) {
        AUI_LOG_ERROR("No component with given key exists. Screen: %s, Key: %s", debugName.c_str(), key.data());
    }

    // Erase the pointer from the map.
    componentMap.erase(mapIt);

    // Remove the component from the vector.
    bool wasErased{false};
    for (unsigned int i = 0; i < components.size(); ++i) {
        if (components[i]->getKey() == key) {
            components.erase(components.begin() + i);
            wasErased = true;
        }
    }

    if (!wasErased) {
        AUI_LOG_ERROR("Found component in map but not in vector. Something bad happened.");
    }
}

std::vector<std::unique_ptr<Component>>& Screen::getComponents()
{
    return components;
}

} // namespace AUI
