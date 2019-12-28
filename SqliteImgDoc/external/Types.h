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

    template <typename t>
    struct Point3dT
    {
        Point3dT() :x(0), y(0), z(0) {}
        Point3dT(t x, t y, t z) :x(x), y(y), z(z) {}
        t x;
        t y;
        t z;
    };

    struct Point3dF : Point3dT<float>
    {
        Point3dF() :Point3dT<float>() {}
        Point3dF(float x, float y, float z) :Point3dT<float>(x, y, z) {}
    };

    struct Point3dD : Point3dT<double>
    {
        Point3dD() :Point3dT<double>() {}
        Point3dD(double x, double y, double z) :Point3dT<double>(x, y, z) {}
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

    template<typename t>
    struct CuboidT
    {
        CuboidT() :x(0), y(0), z(0), w(0), h(0), d(0) {}
        CuboidT(t x, t y, t z, t w, t h, t d)
        {
            if (w < 0 || h < 0 || d < 0)
            {
                throw std::invalid_argument("width and height must be non-negative");
            }

            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
            this->h = h;
            this->d = d;
        }

        t x;
        t y;
        t z;
        t w;
        t h;
        t d;

        bool IsPointInside(const Point3dD& p)
        {
            if (this->x <= p.x && (this->x + this->w) >= p.x && this->y <= p.y && (this->y + this->h) >= p.y && this->z <= p.z && (this->z + this->d) >= p.z)
            {
                return true;
            }

            return false;
        }
    };

    struct CuboidF : CuboidT<float>
    {
        CuboidF() :CuboidT<float>() {}
        CuboidF(float x, float y, float z, float w, float h, float d) :CuboidT<float>(x, y, z, w, h, d) {}
    };

    struct CuboidD : CuboidT<double>
    {
        CuboidD() :CuboidT<double>() {}
        CuboidD(double x, double y, double z, double w, double h, double d) :CuboidT<double>(x, y, z, w, h, d) {}
    };
}
