#include "AUI/Page.h"

namespace AUI {

void Page::addComponent(Component& component) {
    components.push_back(component);
}

} // namespace AUI
