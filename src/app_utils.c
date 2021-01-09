internal v2 GetCursorPosition()
{
    return v2(os->mousePosition.x * app->renderer.screenScale.x,
              os->mousePosition.y * app->renderer.screenScale.y);
}
