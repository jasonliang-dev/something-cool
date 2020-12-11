internal v2 GetCursorPosition()
{
    return v2(os->mousePosition.x * app->screenScale.x, os->mousePosition.y * app->screenScale.y);
}
