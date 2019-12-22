#pragma once
#include <exception>

class SqliteImgDocException : public std::exception
{
public:

    /// Destructor.
    /// Virtual to allow for subclassing.
    virtual ~SqliteImgDocException() throw () {}
};


class SqliteImgDocDbDiscoverException:public SqliteImgDocException
{
public:
    class enum Errortype
    {
        InvalidSchema
    };
};