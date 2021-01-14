internal v2 GetCursorPosition(void)
{
    return os->mousePosition * app->renderer.screenScale;
}

internal v2 GetWindowResolution(void)
{
    return v2((f32)os->windowWidth, (f32)os->windowHeight);
}
