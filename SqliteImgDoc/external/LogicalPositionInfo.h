#pragma once
#include "utilities.h"

namespace SlImgDoc
{
    struct LogicalPositionInfo
    {
        double posX;
        double posY;
        double width;
        double height;
        int pyrLvl;

        bool operator==(const LogicalPositionInfo& rhs) const
        {
            constexpr double DBLEPSILON = 1e-8;
            return this->pyrLvl == rhs.pyrLvl &&
                approximatelyEqual(this->posX, rhs.posX, DBLEPSILON) &&
                approximatelyEqual(this->posY, rhs.posY, DBLEPSILON) &&
                approximatelyEqual(this->width, rhs.width, DBLEPSILON) &&
                approximatelyEqual(this->height, rhs.height, DBLEPSILON);
        }

        bool operator!=(const LogicalPositionInfo& rhs) const
        {
            return !this->operator==(rhs);
        }
    };

    struct LogicalPositionInfo3D
    {
        double posX;
        double posY;
        double posZ;
        double width;
        double height;
        double depth;
        int pyrLvl;

        bool operator==(const LogicalPositionInfo3D& rhs) const
        {
            constexpr double DBLEPSILON = 1e-8;
            return this->pyrLvl == rhs.pyrLvl &&
                approximatelyEqual(this->posX, rhs.posX, DBLEPSILON) &&
                approximatelyEqual(this->posY, rhs.posY, DBLEPSILON) &&
                approximatelyEqual(this->posZ, rhs.posZ, DBLEPSILON) &&
                approximatelyEqual(this->width, rhs.width, DBLEPSILON) &&
                approximatelyEqual(this->height, rhs.height, DBLEPSILON) &&
                approximatelyEqual(this->depth, rhs.depth, DBLEPSILON);
        }

        bool operator!=(const LogicalPositionInfo3D& rhs) const
        {
            return !this->operator==(rhs);
        }
    };
}
