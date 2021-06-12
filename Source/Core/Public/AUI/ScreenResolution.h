#pragma once

namespace AUI {

/**
 * Represents a screen's resolution.
 */
struct ScreenResolution {
    int width{0};

    int height{0};

    /**
     * Compares the width and height of both resolutions.
     */
    bool operator==(const ScreenResolution& rhs) {
        return (width == rhs.width) && (height == rhs.height);
    }

    /**
     * Compares the width and height of both resolutions.
     */
    bool operator!=(const ScreenResolution& rhs) {
        return (width != rhs.width) || (height != rhs.height);
    }

    // TODO: We assume aspect ratio is fixed, so we only compare width.
    //       When support for other aspect ratios is added, we'll have to
    //       first do some math to see what 16:9 resolution fits into the given
    //       resolutions, and compare the width of those.
    friend bool operator<(const ScreenResolution& lhs, const ScreenResolution& rhs) {
        return lhs.width < rhs.width;
    }
};

} // namespace AUI
