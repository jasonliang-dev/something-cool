#if PLATFORM_WIN32
#include <gl/gl.h>
#elif PLATFORM_LINUX
#include <GL/gl.h>
#include <GL/glx.h>
#endif

#include "ext/glext.h"

#define GLProc(name, type) PFN##type##PROC name = NULL;
#include "opengl_procedure_list.h"
#undef GLProc
