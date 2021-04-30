#pragma once

#include "language.h"
#include "geometry.h"

typedef struct Camera3D Camera3D;
struct Camera3D
{
    m4 projection;
    m4 view;
    v3 position;
    v3 forward;
    v3 right;
    f32 yaw;
    f32 pitch;
    f32 sensitivity;
    f32 zNear;
    f32 zFar;
    f32 fov;
};

void Camera3DCreate(Camera3D *camera, v3 position, f32 yaw, f32 pitch);
void Camera3DFreeUpdate(Camera3D *camera);
// void Camera3DFirstPersonUpdate(Camera3D *camera);
