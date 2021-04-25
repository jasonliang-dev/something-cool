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
};

void Camera3DCreate(Camera3D *camera, v3 position, f32 yaw, f32 pitch);
void Camera3DUpdateLook(Camera3D *camera, v2 delta); // updates yaw, pitch and vectors
void Camera3DUpdateViewProj(Camera3D *camera, v2 resolution); // call after updating position
