#pragma once

#include "geometry.h"
#include <glad/gl.h>

void InitRenderer(void);

void BeginDraw(m4 mvp);
void EndDraw(void);
void DrawQuad(m4 transform, v4 texCoords, GLuint texId, v4 color);
