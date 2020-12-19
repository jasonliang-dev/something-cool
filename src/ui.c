internal b32 UI_SpriteButton(texture_t sprite, v2 position)
{
    R_DrawSpriteExt(sprite, position, 0, v2(1, 1), v2(0, 0));

    v4 bounds = v4(position.x, position.y, (f32)sprite.width, (f32)sprite.height);
    if (V4HasPoint(bounds, GetCursorPosition()) && app->mousePress[MouseButton_Left])
    {
        return true;
    }

    return false;
}
