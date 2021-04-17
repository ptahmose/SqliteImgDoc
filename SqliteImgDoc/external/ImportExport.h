#pragma once

// if linking with the static SqliteImgDoc-library, the variable "_SQLITEIMGDOCSTATICLIB" should be defined.
#if !defined(_SQLITEIMGDOCSTATICLIB)

    #ifdef SQLITEIMGDOC_EXPORTS
    #ifdef __GNUC__
    #define SQLITEIMGDOC_API __attribute__ ((visibility ("default")))
    #else
    #define SQLITEIMGDOC_API __declspec(dllexport)
    #endif
    #else
    #ifdef __GNUC__
    #define SQLITEIMGDOC_API
    #else
    #define SQLITEIMGDOC_API __declspec(dllimport)
    #endif
    #endif

#else

    #define SQLITEIMGDOC_API 

#endif

