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

internal v4 UI_GetNextFlexRect(ui_t *ui)
{
    Assert(ui->flexStackCount > 0);

    ui_flex_stack_t *node = ui->flexStack + ui->flexStackCount - 1;

    v4 rect;
    rect.x = node->position.x;
    rect.y = node->position.y;
    rect.width = node->size.x;
    rect.height = node->size.y;

    if (node->direction == UI_FLEX_ROW)
    {
        rect.x += node->progress;
        node->progress += rect.width;
    }
    else if (node->direction == UI_FLEX_COLUMN)
    {
        rect.y += node->progress;
        node->progress += rect.height;
    }
    else
    {
        Assert(!"Invalid code path");
    }

    return rect;
}

internal void UI_PushFlex(ui_t *ui, v2 position, v2 size, ui_flex_direction_t direction)
{
    Assert(ui->flexStackCount < UI_MAX_FLEX_GROUP);

    ui_flex_stack_t *node = ui->flexStack + ui->flexStackCount++;
    node->position = position;
    node->size = size;
    node->direction = direction;
    node->progress = 0.0f;
}

internal void UI_PopFlex(ui_t *ui)
{
    Assert(ui->flexStackCount > 0);
    ui->flexStackCount--;
}

internal f32 UI_SliderExt(ui_t *ui, ui_id_t id, f32 value, v4 rect)
{
    Assert(ui->widgetCount < UI_MAX_WIDGETS);

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

    if (!UI_IDEqual(ui->active, id) && isHot && ui->leftDown)
    {
        ui->active = id;
    }

    if (UI_IDEqual(ui->active, id))
    {
        if (ui->leftDown)
        {
            value = (ui->cursor.x - rect.x) / rect.width;
        }
        else
        {
            ui->active = UI_IDNull();
        }
    }

    ui_widget_t *widget = ui->widgets + ui->widgetCount++;
    widget->type = UI_WIDGET_SLIDER;
    widget->id = id;
    widget->box = rect;
    widget->slider.value = Clampf(value, 0.0f, 1.0f);

    return widget->slider.value;
}

internal f32 UI_Slider(ui_t *ui, ui_id_t id, f32 value)
{
    v4 rect = UI_GetNextFlexRect(ui);
    return UI_SliderExt(ui, id, value, rect);
}

internal b32 UI_ButtonExt(ui_t *ui, ui_id_t id, char *text, v4 rect)
{
    Assert(ui->widgetCount < UI_MAX_WIDGETS);

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
    v4 rect = UI_GetNextFlexRect(ui);
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
    ui->flexStackCount = 0;
}

internal void UI_EndFrame(ui_t *ui)
{
    for (u32 i = 0; i < ui->widgetCount; i++)
    {
        ui_widget_t *widget = ui->widgets + i;

        widget->tHot += ((f32)UI_IDEqual(ui->hot, widget->id) - widget->tHot) * 0.05f;
        widget->tActive += ((f32)UI_IDEqual(ui->active, widget->id) - widget->tActive) * 0.05f;

        switch (widget->type)
        {
        case UI_WIDGET_BUTTON: {
            f32 col = 0.6f + widget->tHot * 0.4f - widget->tActive * 0.5f;
            R_DrawRect(v4(col, col, col, 1), v2(widget->box.x, widget->box.y),
                       v2(widget->box.width, widget->box.height));
        }
        break;
        case UI_WIDGET_SLIDER:
            R_DrawRect(v4(0.6f, 0.6f, 0.6f, 1), v2(widget->box.x, widget->box.y),
                       v2(widget->box.width, widget->box.height));
            R_DrawRect(v4(1, 1, 1, 1), v2(widget->box.x, widget->box.y),
                       v2(widget->box.width * widget->slider.value, widget->box.height));
            break;
        default:
            Assert(!"Unhandled UI widget type");
            break;
        }
    }
}
