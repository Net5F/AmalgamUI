#include "AUI/Screen.h"

namespace AUI {

Screen::Screen(const std::string& inDebugName)
: debugName{inDebugName}
, componentMap()
{
}

void Screen::registerComponent(Component& component)
{
    // If the key is empty or we already have a component with that key, fail.
    const entt::hashed_string& key = component.getKey();
    if ((key == entt::hashed_string{""})
        || (componentMap.find(key.value()) != componentMap.end())) {
        AUI_LOG_ERROR("Tried to add component with key that is empty or already in use. Screen: %s, Key: %s", debugName.c_str(), key.data());
    }

    // Add it to the map.
    componentMap[key.value()] = &component;
}

void Screen::unregisterComponent(const entt::hashed_string& key)
{
    // If we don't have a component with that key, fail.
    auto mapIt = componentMap.find(key.value());
    if (mapIt == componentMap.end()) {
        AUI_LOG_ERROR("No component with given key exists. Screen: %s, Key: %s", debugName.c_str(), key.data());
    }

    // Erase the pointer from the map.
    componentMap.erase(mapIt);
}

void Screen::render()
{
    // Rendering is left up to derived classes.
}

} // namespace AUI
