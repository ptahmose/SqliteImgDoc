#pragma once
#include <exception>
#include <string>

namespace SlImgDoc
{
    class SqliteImgDocException : public std::runtime_error
    {
    public:
        /// Constructor of the SqliteImgDocException.
        /// \param szErrMsg Message describing the error.
        explicit SqliteImgDocException(const char* szErrMsg)
            : std::runtime_error(szErrMsg) { }
    };


    class SqliteImgDocDbDiscoverException :public SqliteImgDocException
    {
    public:
        enum class ErrorType
        {
            UnsuitableDb,
        };

        explicit  SqliteImgDocDbDiscoverException(const std::string& errMsg, SqliteImgDocDbDiscoverException::ErrorType type)
            : SqliteImgDocDbDiscoverException(errMsg.c_str(), type) {}

        explicit  SqliteImgDocDbDiscoverException(const char* szErrMsg, SqliteImgDocDbDiscoverException::ErrorType type)
            : SqliteImgDocException(szErrMsg), errorType(type) {}
    private:
        ErrorType errorType;
    };
}