#if BUILD_WIN32
#include <windows.h>
#include <gl/gl.h>
#else
#error "OpenGL not supported for this platform."
#endif
