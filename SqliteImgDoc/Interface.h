#pragma once

#include <string>
#include <unordered_set>

namespace SlImgDoc
{
    class CreateOptions
    {
    public:
        std::string dbFilename;

        std::unordered_set<char> dimensions;
    };

    class IDbWrite
    {
    public:
        virtual ~IDbWrite()=0;
    };


    class IDbFactory
    {
    public:
        static IDbWrite* CreateNew(const CreateOptions& opts);
    };

    
}