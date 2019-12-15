#pragma once

#define _SQLITEIMGDOCSTATICLIB

// if linking with the static libCZI-library, the variable "_LIBCZISTATICLIB" should be defined.
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

