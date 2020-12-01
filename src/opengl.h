#if BUILD_WIN32
#include <gl/gl.h>
#elif BUILD_LINUX
#include <GL/gl.h>
#include <GL/glx.h>
#else
#error "OpenGL includes for platform not supported."
#endif

#include "ext/glext.h"

#define GLProc(name, type) PFN##type##PROC name = NULL;
#include "opengl_procedure_list.h"
#undef GLProc
