#pragma once

#include <stdexcept>
#include <ctgmath>

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

    template <typename t> struct Vector3dT;
    template<typename T> struct Plane_NormalAndDist;

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

        bool IsPointInside(const Point3dD& p) const
        {
            if (this->x <= p.x && (this->x + this->w) >= p.x && this->y <= p.y && (this->y + this->h) >= p.y && this->z <= p.z && (this->z + this->d) >= p.z)
            {
                return true;
            }

            return false;
        }

        Point3dT<t> CenterPoint() const
        {
            return Point3dT<t>(this->x + this->w / 2, this->y + this->h / 2, this->z + this->d / 2);
        }

        static bool DoIntersect(const SlImgDoc::CuboidT<t>& aabb, const SlImgDoc::Plane_NormalAndDist<t>& plane);
     /*   {
            // -> https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
            const auto centerAabb = aabb.CenterPoint();
            const Vector3dT<t> aabbExtents = Vector3dD(aabb.w / 2, aabb.h / 2, aabb.d / 2);

            // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
            const auto r = aabbExtents.x * fabs(plane.normal.x) + aabbExtents.y * fabs(plane.normal.y) + aabbExtents.z * fabs(plane.normal.z);

            // Compute distance of box center from plane
            const auto s = Vector3dT<t>::Dot(plane.normal, centerAabb) - plane.distance;

            // Intersection occurs when distance s falls within [-r,+r] interval
            return fabs(s) <= r;
        }*/

        bool DoesIntersectWith(const SlImgDoc::Plane_NormalAndDist<t>& plane) const
        {
            return DoIntersect(*this, plane);
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

    template <typename t>
    struct Vector3dT
    {
        Vector3dT() :x(0), y(0), z(0) {}
        Vector3dT(t x, t y, t z) :x(x), y(y), z(z) {}
        Vector3dT(const Point3dT<t> p) : Vector3dT(p.x, p.y, p.z) {}
        t x;
        t y;
        t z;

        Vector3dT<t> Normalize() const
        {
            t absVal = this->AbsoluteValue();
            return Vector3dT(this->x / absVal, this->y / absVal, this->z / absVal);
        }

        t AbsoluteValueSquared() const
        {
            return this->x * this->x + this->y * this->y + this->z * this->z;
        }

        t AbsoluteValue() const
        {
            return std::sqrt(this->AbsoluteValueSquared());
        }

        static Vector3dT<t> Cross(const Vector3dT<t>& vectorA, const Vector3dT<t>& vectorB)
        {
            return Vector3dT
            {
              vectorA.y * vectorB.z - vectorA.z * vectorB.y,
              vectorA.z * vectorB.x - vectorA.x * vectorB.z,
              vectorA.x * vectorB.y - vectorA.y * vectorB.x
            };
        }

        static t Dot(const Vector3dT<t>& vectorA, const Vector3dT<t>& vectorB)
        {
            return vectorA.x * vectorB.x + vectorA.y * vectorB.y + vectorA.z * vectorB.z;
        }
    };

    struct Vector3dF : Vector3dT<float>
    {
        Vector3dF() :Vector3dT<float>() {}
        Vector3dF(float x, float y, float z) :Vector3dT<float>(x, y, z) {}
    };

    struct Vector3dD : Vector3dT<double>
    {
        Vector3dD() :Vector3dT<double>() {}
        Vector3dD(double x, double y, double z) :Vector3dT<double>(x, y, z) {}
    };

    /// Parametrization of a plane, parametrized as a normal-vector and the distance to the origin.
    /// The normal must be normalized.
    /// The equation of the plane is: dot( x, normal) = distance.
    template<typename T>
    struct Plane_NormalAndDist
    {
        Vector3dT<T> normal;
        T distance;

        Plane_NormalAndDist() :distance(0) {}
        Plane_NormalAndDist(const Vector3dT<T>& n, T d) :normal(n), distance(d) {}

        static Plane_NormalAndDist<T> FromThreePoints(Point3dT<T> a, Point3dT<T> b, Point3dT<T> c)
        {
            auto n = Vector3dT<T>::Cross(Vector3dT<T>(b.x - a.x, b.y - a.y, b.z - a.z), Vector3dT<T>(c.x - a.x, c.y - a.y, c.z - a.z)).Normalize();
            auto dist = Vector3dT<T>::Dot(n, Vector3dT<T>(a));
            return Plane_NormalAndDist<T>(n, dist);
        }
    };

    struct Plane_NormalAndDistF : Plane_NormalAndDist<float>
    {};

    struct Plane_NormalAndDistD : Plane_NormalAndDist<double>
    {
        Plane_NormalAndDistD() = default;
        Plane_NormalAndDistD(const Vector3dD& n, double d) :Plane_NormalAndDist<double>(n, d) {}
        Plane_NormalAndDistD(const Vector3dT<double>& n, double d) :Plane_NormalAndDist<double>(n, d) {}
    };


    /*static*/template<typename t> inline bool CuboidT<t>::DoIntersect(const SlImgDoc::CuboidT<t>& aabb, const SlImgDoc::Plane_NormalAndDist<t>& plane)
    {
        // -> https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
        const auto centerAabb = aabb.CenterPoint();
        const Vector3dT<t> aabbExtents = Vector3dD(aabb.w / 2, aabb.h / 2, aabb.d / 2);

        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        const auto r = aabbExtents.x * fabs(plane.normal.x) + aabbExtents.y * fabs(plane.normal.y) + aabbExtents.z * fabs(plane.normal.z);

        // Compute distance of box center from plane
        const auto s = Vector3dT<t>::Dot(plane.normal, centerAabb) - plane.distance;

        // Intersection occurs when distance s falls within [-r,+r] interval
        return fabs(s) <= r;
    }

}
