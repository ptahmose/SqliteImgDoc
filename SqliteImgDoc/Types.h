#pragma once

namespace SlImgDoc
{
    struct RectangleF
    {
        float x;
        float y;
        float w;
        float h;
    };

    struct PointF
    {
        float x;
        float y;
    };

    struct LineThruTwoPoints
    {
        PointF a;
        PointF b;
    };
}
