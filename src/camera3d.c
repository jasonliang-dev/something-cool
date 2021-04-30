#include "camera3d.h"
#include "input.h"
#include "window.h"
#include <math.h>
#include <string.h>

static void UpdateCameraVectors(Camera3D *camera)
{
    f32 x = cosf(camera->yaw) * cosf(camera->pitch);
    f32 y = sinf(camera->pitch);
    f32 z = sinf(camera->yaw) * cosf(camera->pitch);
    camera->forward = V3Normalize(v3(x, y, z));
    camera->right = V3Normalize(V3Cross(camera->forward, v3(0.0f, 1.0f, 0.0f)));
}

void Camera3DCreate(Camera3D *camera, v3 position, f32 yaw, f32 pitch)
{
    camera->position = position;
    camera->yaw = yaw;
    camera->pitch = pitch;

    camera->sensitivity = 0.005f;
    camera->fov = PI / 4;
    camera->zNear = 0.01f;
    camera->zFar = 1000.0f;

    UpdateCameraVectors(camera);
}

static void UpdateLook(Camera3D *camera, v2 delta)
{
    f32 justUnderHalfPi = PI / 2.0f - 0.01f;

    camera->yaw += delta.x * camera->sensitivity;
    camera->pitch =
        Clamp(camera->pitch - delta.y * camera->sensitivity, -justUnderHalfPi, justUnderHalfPi);

    UpdateCameraVectors(camera);
}

static m4 Perspective(f32 fov, v2 resolution, f32 zNear, f32 zFar)
{
    float depth = zFar - zNear;

    m4 result;
    memset(&result, 0, sizeof(m4));

    result.elements[2][2] = zFar * (1 / depth);
    result.elements[1][1] = 1 / tanf(0.5f * fov);
    result.elements[0][0] =
        -result.elements[1][1] / (resolution.width / resolution.height); // right hand, negative

    result.elements[3][2] = -(zFar * zNear) * (1 / depth);
    result.elements[2][3] = 1;

    return result;
}

static m4 LookAt(Camera3D *camera)
{
    m4 result;

    v3 up = V3Normalize(V3Cross(camera->right, camera->forward));

    result.elements[0][0] = camera->right.x;
    result.elements[0][1] = up.x;
    result.elements[0][2] = -camera->forward.x;
    result.elements[0][3] = 0.0f;

    result.elements[1][0] = camera->right.y;
    result.elements[1][1] = up.y;
    result.elements[1][2] = -camera->forward.y;
    result.elements[1][3] = 0.0f;

    result.elements[2][0] = camera->right.z;
    result.elements[2][1] = up.z;
    result.elements[2][2] = -camera->forward.z;
    result.elements[2][3] = 0.0f;

    result.elements[3][0] = V3Dot(camera->right, camera->position);
    result.elements[3][1] = V3Dot(up, camera->position);
    result.elements[3][2] = -V3Dot(camera->forward, camera->position);
    result.elements[3][3] = 1.0f;

    return result;
}

static void UpdateViewProj(Camera3D *camera, v2 resolution)
{
    camera->projection = Perspective(camera->fov, resolution, camera->zNear, camera->zFar);
    camera->view = LookAt(camera);
}

void Camera3DFreeUpdate(Camera3D *camera)
{
    v2 look = v2(0, 0);
    f32 sensitivity = 6.0f;
    if (KeyDown(Key_Up))
        look.y += sensitivity;
    if (KeyDown(Key_Down))
        look.y -= sensitivity;
    if (KeyDown(Key_Left))
        look.x -= sensitivity;
    if (KeyDown(Key_Right))
        look.x += sensitivity;

    UpdateLook(camera, look);

    f32 moveSpeed = 0.2f;
    if (KeyDown(Key_W))
        camera->position = V3PlusV3(camera->position, V3xF32(camera->forward, moveSpeed));
    if (KeyDown(Key_S))
        camera->position = V3PlusV3(camera->position, V3xF32(camera->forward, -moveSpeed));
    if (KeyDown(Key_A))
        camera->position = V3PlusV3(camera->position, V3xF32(camera->right, -moveSpeed));
    if (KeyDown(Key_D))
        camera->position = V3PlusV3(camera->position, V3xF32(camera->right, moveSpeed));

    if (KeyDown(Key_Space))
        camera->position.y -= moveSpeed;
    if (KeyDown(Key_LeftShift))
        camera->position.y += moveSpeed;

    UpdateViewProj(camera, WindowV2());
}
