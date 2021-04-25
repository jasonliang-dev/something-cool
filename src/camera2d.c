#include "camera2d.h"
#include <string.h>

Camera2D Camera2DCreate(void)
{
    Camera2D camera;
    camera.position = v2(0, 0);
    return camera;
}

static m4 Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar)
{
    m4 result;
    memset(&result, 0, sizeof(m4));

    f32 rl = 1.0f / (right - left);
    f32 tb = 1.0f / (top - bottom);
    f32 fn = -1.0f / (zFar - zNear);

    result.elements[0][0] = 2.0f * rl;
    result.elements[1][1] = 2.0f * tb;
    result.elements[2][2] = 2.0f * fn;
    result.elements[3][0] = -(right + left) * rl;
    result.elements[3][1] = -(top + bottom) * tb;
    result.elements[3][2] = (zFar + zNear) * fn;
    result.elements[3][3] = 1.0f;

    return result;
}

void Camera2DUpdate(Camera2D *camera, v2 resolution)
{
    camera->projection = Ortho(0, resolution.x, resolution.y, 0, -1, 1);
    camera->view = M4Translate(M4Identity(), v3(camera->position.x, camera->position.y, 0));
}
