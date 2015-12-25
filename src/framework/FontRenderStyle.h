#pragma once

namespace Acidrain {

    const float TEXT_DOWNSAMPLE_FACTOR = 1.8f;

    struct FontRenderStyle {
        int outlineSize = 1;
        int shadowOffsetX = 3;
        int shadowOffsetY = 3;
    };

    static FontRenderStyle DEFAULT_FONT_RENDER_STYLE;
}