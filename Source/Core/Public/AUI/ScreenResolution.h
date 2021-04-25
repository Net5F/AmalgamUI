#pragma once

namespace AUI {

/**
 * Represents a screen's resolution.
 */
struct ScreenResolution {
    int width{0};

    int height{0};

    bool operator==(const ScreenResolution& rhs) {
        return (width == rhs.width) && (height == rhs.height);
    }

    bool operator!=(const ScreenResolution& rhs) {
        return (width != rhs.width) || (height != rhs.height);
    }
};

} // namespace AUI
