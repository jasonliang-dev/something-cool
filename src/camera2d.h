#pragma once

#include "geometry.h"
#include "language.h"

typedef struct Camera2D Camera2D;
struct Camera2D
{
    m4 projection;
    v2 position;
};

Camera2D Camera2DCreate(void);
void Camera2DUpdate(Camera2D *camera, v2 bottomRight);
