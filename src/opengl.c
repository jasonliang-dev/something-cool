internal void LoadOpenGLProcedures(void)
{
#define GLProc(name, type) name = (PFN##type##PROC)os->LoadOpenGLProcedure(#name);
#include "opengl_procedure_list.h"
#undef GLProc
}
