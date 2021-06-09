#include "ui.h"
#include "assets.h"
#include "font.h"
#include "input.h"
#include "renderer.h"
#include <assert.h>
#include <string.h>

enum
{
    UI_TEXT_VISIBLE = 1 << 0,
    UI_LIFT_WHEN_HOT = 1 << 1,
    UI_LUMINATE_WHEN_HOT = 1 << 2,
};

typedef struct UIElement UIElement;
struct UIElement
{
    UIID id;
    i32 flags;
    v4 rect;

    v4 color;
    const char *text;
};

static UIElement *AllocateUIElement(UIID id, i32 flags, v4 rect)
{
    assert(g_UI.elementCount < UI_ELEMENTS_MAX);

    UIElement *element = g_UI.elements + g_UI.elementCount++;
    memset(element, 0, sizeof(UIElement));

    element->id = id;
    element->flags = flags;
    element->rect = rect;

    return element;
}

static struct
{
    UIID hot;

    UIElement elements[UI_ELEMENTS_MAX];
    i32 elementCount;
} g_UI;

void InitUI(void)
{
    memset(&g_UI, 0, sizeof(g_UI));
}

void UIBegin(void)
{
    g_UI.elementCount = 0;
}

void DrawUI(m4 projection)
{
    BeginDraw(fnt_Primary.texture, projection);
    for (i32 i = 0; i < g_UI.elementCount; ++i)
    {
        UIElement *element = g_UI.elements + i;

        if (element->flags & UI_LIFT_WHEN_HOT && element->id == g_UI.hot)
        {
            element->rect.y -= 2;
        }

        if (element->flags & UI_LUMINATE_WHEN_HOT && element->id != g_UI.hot)
        {
            element->color.x *= 0.8f;
            element->color.y *= 0.8f;
            element->color.z *= 0.8f;
        }

        if (element->flags & UI_TEXT_VISIBLE)
        {
            DrawFont(element->text, fnt_Primary, v2(element->rect.x, element->rect.y),
                     element->color);
        }
    }
    EndDraw();
}

b32 UITextButton(UIID id, const char *text, v2 pos)
{
    UIElement *element =
        AllocateUIElement(id, UI_TEXT_VISIBLE | UI_LIFT_WHEN_HOT | UI_LUMINATE_WHEN_HOT,
                          v4(pos.x, pos.y, 150, 32));

    element->text = text;
    element->color = v4(1, 1, 1, 1);

    v2 mouse = MousePos();
    if (element->rect.x <= mouse.x && mouse.x <= element->rect.x + element->rect.z &&
        element->rect.y <= mouse.y && mouse.y <= element->rect.y + element->rect.w)
    {
        g_UI.hot = id;
    }
    else if (g_UI.hot == id)
    {
        g_UI.hot = 0;
    }

    return g_UI.hot == id && MouseClicked(GLFW_MOUSE_BUTTON_LEFT);
}
