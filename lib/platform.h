#pragma once

#if defined(__linux__)
# define ACTO_LINUX
# define ACTO_UNIX
#elif defined(__APPLE__)
# define ACTO_DARWIN
# define ACTO_UNIX
#elif defined(_WIN64)
# define ACTO_WIN
# define ACTO_WIN64
#else
# error "Unsupported target platform"
#endif

#if defined(_MSC_VER)
# if !defined(_MT)
#  error "Multithreaded mode not defined. Use /MD or /MT compiler options."
# endif

# ifdef ACTO_EXPORT
#  define ACTO_API __declspec(dllexport)
# elif ACTO_IMPORT
#  define ACTO_API __declspec(dllimport)
# else
#  define ACTO_API
# endif
#else
# define ACTO_API
#endif
