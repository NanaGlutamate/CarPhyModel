
#ifndef COMM_EXPORT_H
#define COMM_EXPORT_H

#ifdef COMM_STATIC_DEFINE
#  define COMM_EXPORT
#  define COMM_NO_EXPORT
#else
#  ifndef COMM_EXPORT
#    ifdef comm_EXPORTS
        /* We are building this library */
#      define COMM_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define COMM_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef COMM_NO_EXPORT
#    define COMM_NO_EXPORT 
#  endif
#endif

#ifndef COMM_DEPRECATED
#  define COMM_DEPRECATED __declspec(deprecated)
#endif

#ifndef COMM_DEPRECATED_EXPORT
#  define COMM_DEPRECATED_EXPORT COMM_EXPORT COMM_DEPRECATED
#endif

#ifndef COMM_DEPRECATED_NO_EXPORT
#  define COMM_DEPRECATED_NO_EXPORT COMM_NO_EXPORT COMM_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef COMM_NO_DEPRECATED
#    define COMM_NO_DEPRECATED
#  endif
#endif

#endif /* COMM_EXPORT_H */
