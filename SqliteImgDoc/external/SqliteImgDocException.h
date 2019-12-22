#pragma once
#include <exception>
#include <string>

namespace SlImgDoc
{
    class SqliteImgDocException : public std::runtime_error
    {
    private:
        std::exception_ptr innerException;
    public:
        /// Constructor of the SqliteImgDocException.
        /// \param szErrMsg Message describing the error.
        explicit SqliteImgDocException(const char* szErrMsg)
            : std::runtime_error(szErrMsg) { }

        explicit SqliteImgDocException(const char* szErrMsg, const std::exception& innerException)
            : SqliteImgDocException(szErrMsg)
        {
            this->innerException = std::make_exception_ptr(innerException);
        }
    };

    class SqliteImgDocInvalidArgumentException : public SqliteImgDocException
    {
    public:
        /// Constructor of the SqliteImgDocInvalidArgumentException.
        /// \param szErrMsg Message describing the error.
        explicit SqliteImgDocInvalidArgumentException(const char* szErrMsg)
            : SqliteImgDocException(szErrMsg) { }

        explicit SqliteImgDocInvalidArgumentException(const char* szErrMsg, const std::exception& innerException)
            : SqliteImgDocException(szErrMsg, innerException) { }

        /// Constructor of the SqliteImgDocInvalidArgumentException.
       /// \param errMsg Message describing the error.
        explicit SqliteImgDocInvalidArgumentException(const std::string& errMsg)
            : SqliteImgDocInvalidArgumentException(errMsg.c_str()) { }

        explicit SqliteImgDocInvalidArgumentException(const std::string& errMsg, const std::exception& innerException)
            : SqliteImgDocInvalidArgumentException(errMsg.c_str(), innerException) { }
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

        explicit  SqliteImgDocDbDiscoverException(const std::string& errMsg, SqliteImgDocDbDiscoverException::ErrorType type, const std::exception& innerException)
            : SqliteImgDocDbDiscoverException(errMsg.c_str(), type, innerException) {}

        explicit  SqliteImgDocDbDiscoverException(const char* szErrMsg, SqliteImgDocDbDiscoverException::ErrorType type)
            : SqliteImgDocException(szErrMsg), errorType(type) {}

        explicit  SqliteImgDocDbDiscoverException(const char* szErrMsg, SqliteImgDocDbDiscoverException::ErrorType type, const std::exception& innerException)
            : SqliteImgDocException(szErrMsg, innerException), errorType(type) {}


        ErrorType GetErrorType() const { return this->errorType; }
    private:
        ErrorType errorType;
    };
}