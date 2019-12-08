#pragma once

#include <stdexcept>

namespace SlImgDoc
{
    template <typename t>
    struct PointT
    {
        PointT() :x(0), y(0) {}
        PointT(t x, t y) :x(x), y(y) {}
        t x;
        t y;
    };

    struct PointF : PointT<float>
    {
        PointF() :PointT<float>() {}
        PointF(float x, float y) :PointT<float>(x, y) {}
    };

    struct PointD : PointT<double>
    {
        PointD() :PointT<double>() {}
        PointD(double x, double y) :PointT<double>(x, y) {}
    };

    template<typename t>
    struct RectangleT
    {
        RectangleT() :x(0), y(0), w(0), h(0) {}
        RectangleT(t x, t y, t w, t h)
        {
            if (w < 0 || h < 0)
            {
                throw std::invalid_argument("width and height must be non-negative");
            }

            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
        }

        t x;
        t y;
        t w;
        t h;

        bool IsPointInside(const PointD& p)
        {
            if (this->x <= p.x && (this->x + this->w) >= p.x && this->y <= p.y && (this->y + this->h) >= p.y)
            {
                return true;
            }

            return false;
        }
    };

    struct RectangleF : RectangleT<float>
    {
        RectangleF() :RectangleT<float>() {}
        RectangleF(float x, float y, float w, float h) :RectangleT<float>(x, y, w, h) {}
    };

    struct RectangleD : RectangleT<double>
    {
        RectangleD() :RectangleT<double>() {}
        RectangleD(double x, double y, double w, double h) :RectangleT<double>(x, y, w, h) {}
    };

    struct LineThruTwoPointsF
    {
        PointF a;
        PointF b;
    };

    struct LineThruTwoPointsD
    {
        PointD a;
        PointD b;
    };
}
