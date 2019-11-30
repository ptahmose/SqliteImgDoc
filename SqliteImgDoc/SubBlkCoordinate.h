#pragma once

class ISubBlkCoordinate
{
public:
    virtual bool TryGetCoordinate(char dim, int* coordVal) const = 0;
};
