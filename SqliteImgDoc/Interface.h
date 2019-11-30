#pragma once

#include <string>
#include <unordered_set>
#include "ImportExport.h"

namespace SlImgDoc
{
    class CreateOptions
    {
    public:
        std::string dbFilename;

        std::unordered_set<char> dimensions;
    };

    class SQLITEIMGDOC_API IDbWrite
    {
    public:
        virtual ~IDbWrite() {};
    };


    class SQLITEIMGDOC_API IDbFactory
    {
    public:
        static IDbWrite* CreateNew(const CreateOptions& opts);
    };

    
}