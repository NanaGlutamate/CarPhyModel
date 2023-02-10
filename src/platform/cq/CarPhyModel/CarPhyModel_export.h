
#ifndef CARPHYMODEL_EXPORT_H
#define CARPHYMODEL_EXPORT_H

#ifdef CARPHYMODEL_STATIC_DEFINE
#  define CARPHYMODEL_EXPORT
#  define CARPHYMODEL_NO_EXPORT
#else
#  ifndef CARPHYMODEL_EXPORT
#    ifdef CarPhyModel_EXPORTS
        /* We are building this library */
#      define CARPHYMODEL_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define CARPHYMODEL_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef CARPHYMODEL_NO_EXPORT
#    define CARPHYMODEL_NO_EXPORT 
#  endif
#endif

#ifndef CARPHYMODEL_DEPRECATED
#  define CARPHYMODEL_DEPRECATED __declspec(deprecated)
#endif

#ifndef CARPHYMODEL_DEPRECATED_EXPORT
#  define CARPHYMODEL_DEPRECATED_EXPORT CARPHYMODEL_EXPORT CARPHYMODEL_DEPRECATED
#endif

#ifndef CARPHYMODEL_DEPRECATED_NO_EXPORT
#  define CARPHYMODEL_DEPRECATED_NO_EXPORT CARPHYMODEL_NO_EXPORT CARPHYMODEL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef CARPHYMODEL_NO_DEPRECATED
#    define CARPHYMODEL_NO_DEPRECATED
#  endif
#endif

#endif /* CARPHYMODEL_EXPORT_H */
