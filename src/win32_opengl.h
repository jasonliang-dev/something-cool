#pragma once

#include "language.h"
#include <windows.h>

typedef BOOL(WINAPI *PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc, const int *piAttribIList,
                                                     const FLOAT *pfAttribFList, UINT nMaxFormats,
                                                     int *piFormats, UINT *nNumFormats);
extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

typedef HGLRC(WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hdc, HGLRC hShareContext,
                                                         const int *attribList);
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int);
extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

const void *W32_GetProcAddress(const char *name);
b32 W32_InitOpenGL(HDC hdc);
