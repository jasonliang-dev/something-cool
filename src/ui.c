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

internal ui_id_t UI_IDNull(void)
{
    ui_id_t id = {0, 0};
    return id;
}

internal b32 UI_IDEqual(ui_id_t a, ui_id_t b)
{
    return (a.primary == b.primary) && (a.secondary == b.secondary);
}

internal ui_id_t UI_MakeID(u32 primary, u32 secondary)
{
    ui_id_t id = {primary, secondary};
    return id;
}

internal v4 UI_GetNextAutoLayoutRect(ui_t *ui)
{
    (void)ui;
    return v4(0, 0, 0, 0);
}

internal b32 UI_ButtonExt(ui_t *ui, ui_id_t id, char *text, v4 rect)
{
    (void)text;

    b32 isMouseOver = V4HasPoint(rect, ui->cursor);
    b32 isHot = UI_IDEqual(ui->hot, id);

    if (!isHot && isMouseOver)
    {
        ui->hot = id;
    }
    else if (isHot && !isMouseOver)
    {
        ui->hot = UI_IDNull();
    }

    b32 triggered = false;

    if (UI_IDEqual(ui->active, id) && !ui->leftDown)
    {
        triggered = UI_IDEqual(ui->hot, id);
        ui->active = UI_IDNull();
    }
    else if (isHot && ui->leftDown)
    {
        ui->active = id;
    }

    ui_widget_t *widget = ui->widgets + ui->widgetCount++;
    widget->type = UI_WIDGET_BUTTON;
    widget->id = id;
    widget->box = rect;

    return triggered;
}

internal b32 UI_Button(ui_t *ui, ui_id_t id, char *text)
{
    v4 rect = UI_GetNextAutoLayoutRect(ui);
    return UI_ButtonExt(ui, id, text, rect);
}

internal void UI_Init(ui_t *ui)
{
    ui->hot = UI_IDNull();
    ui->active = UI_IDNull();
}

internal void UI_BeginFrame(ui_t *ui, ui_input_t *input)
{
    ui->cursor = input->cursor;
    ui->leftDown = input->leftDown;
    ui->rightDown = input->rightDown;

    ui->widgetCount = 0;
}

internal void UI_EndFrame(ui_t *ui, ui_input_t *input)
{
    (void)input;

    for (u32 i = 0; i < ui->widgetCount; i++)
    {
        ui_widget_t *widget = ui->widgets + i;

        f32 deltaTime = 0.016f;
        widget->tHot += ((f32)UI_IDEqual(ui->hot, widget->id) - widget->tHot) * deltaTime * 4.0f;
        widget->tActive += ((f32)UI_IDEqual(ui->active, widget->id) - widget->tActive) * deltaTime * 4.0f;

        switch (widget->type)
        {
        case UI_WIDGET_BUTTON: {
            f32 col = 0.6f + widget->tHot * 0.4f - widget->tActive * 0.5f;
            R_DrawRect(v4(col, col, col, col), v2(widget->box.x, widget->box.y),
                       v2(widget->box.width, widget->box.height));
        }
        break;
        default:
            Assert(false);
            break;
        }
    }
}
