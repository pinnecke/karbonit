
#ifndef UBJS_EXPORT_H
#define UBJS_EXPORT_H

#ifdef UBJS_EXPORTS_BUILT_AS_STATIC
#  define UBJS_EXPORT
#  define UBJS_NO_EXPORT
#else
#  ifndef UBJS_EXPORT
#    ifdef ubjsc_EXPORTS
        /* We are building this library */
#      define UBJS_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define UBJS_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef UBJS_NO_EXPORT
#    define UBJS_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef UBJS_DEPRECATED
#  define UBJS_DEPRECATED 
#endif

#ifndef UBJS_DEPRECATED_EXPORT
#  define UBJS_DEPRECATED_EXPORT UBJS_EXPORT UBJS_DEPRECATED
#endif

#ifndef UBJS_DEPRECATED_NO_EXPORT
#  define UBJS_DEPRECATED_NO_EXPORT UBJS_NO_EXPORT UBJS_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef UBJSC_NO_DEPRECATED
#    define UBJSC_NO_DEPRECATED
#  endif
#endif

#endif /* UBJS_EXPORT_H */
