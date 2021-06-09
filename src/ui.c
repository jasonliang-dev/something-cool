// #define STB_TEXTEDIT_IMPLEMENTATION

//    STB_TEXTEDIT_STRING               the type of object representing a string being edited,
//                                      typically this is a wrapper object with other data you need
// #define STB_TEXTEDIT_STRING char *
//    STB_TEXTEDIT_STRINGLEN(obj)       the length of the string (ideally O(1))
// #define STB_TEXTEDIT_STRINGLEN(obj) strlen(obj)
//    STB_TEXTEDIT_LAYOUTROW(&r,obj,n)  returns the results of laying out a line of characters
//                                        starting from character #n (see discussion below)
// #define STB_TEXTEDIT_LAYOUTROW
//    STB_TEXTEDIT_GETWIDTH(obj,n,i)    returns the pixel delta from the xpos of the i'th character
//                                        to the xpos of the i+1'th char for a line of characters
//                                        starting at character #n (i.e. accounts for kerning
//                                        with previous char)
//    STB_TEXTEDIT_KEYTOTEXT(k)         maps a keyboard input to an insertable character
//                                        (return type is int, -1 means not valid to insert)
//    STB_TEXTEDIT_GETCHAR(obj,i)       returns the i'th character of obj, 0-based
//    STB_TEXTEDIT_NEWLINE              the character returned by _GETCHAR() we recognize
//                                        as manually wordwrapping for end-of-line positioning
//
//    STB_TEXTEDIT_DELETECHARS(obj,i,n)      delete n characters starting at i
//    STB_TEXTEDIT_INSERTCHARS(obj,i,c*,n)   insert n characters at i (pointed to by STB_TEXTEDIT_CHARTYPE*)
//
//    STB_TEXTEDIT_K_SHIFT       a power of two that is or'd in to a keyboard input to represent the shift key

#include "ui.h"
#include "assets.h"
#include "font.h"
#include "input.h"
#include "renderer.h"
#include <assert.h>
#include <math.h>
#include <string.h>
// #include <stb_textedit.h>

#define UI_ELEMENTS_MAX 128
#define UI_AUTO_LAYOUT_STACK_MAX 16

enum
{
    UI_TEXT_VISIBLE = 1 << 0,
    UI_LIFT_WHEN_HOT = 1 << 1,
    UI_HOT_COLOR = 1 << 2,
    UI_ACTIVE_COLOR = 1 << 3,
    UI_BACKGROUND_VISIBLE = 1 << 4,
    UI_TEXT_CURSOR = 1 << 5,
};

typedef struct UIElement UIElement;
struct UIElement
{
    UIID id;
    i32 flags;
    v4 rect;

    v4 foreground;
    v4 background;

    v4 foregroundHot;
    v4 backgroundHot;

    v4 foregroundActive;
    v4 backgroundActive;

    const char *text;
};

typedef struct UIAutoLayout UIAutoLayout;
struct UIAutoLayout
{
    i32 direction;
    v2 progress;
    f32 gap;
};

static struct
{
    UIID hot;
    UIID active;

    f32 time;

    UIAutoLayout autoLayoutStack[UI_AUTO_LAYOUT_STACK_MAX];
    i32 autoLayoutCount;

    UIElement elements[UI_ELEMENTS_MAX];
    i32 elementCount;
} g_UI;

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

void InitUI(void)
{
    memset(&g_UI, 0, sizeof(g_UI));
}

void UIBegin(f32 deltaTime)
{
    g_UI.elementCount = 0;
    g_UI.autoLayoutCount = 0;

    g_UI.time += deltaTime;
}

static UIAutoLayout *AutoLayoutStackPeek(void)
{
    if (g_UI.autoLayoutCount == 0)
    {
        return NULL;
    }

    return g_UI.autoLayoutStack + g_UI.autoLayoutCount - 1;
}

static void GetNextLayoutPosition(v4 *rect)
{
    UIAutoLayout *peek = AutoLayoutStackPeek();

    if (!peek)
    {
        return;
    }

    rect->y += peek->progress.y;
    rect->x += peek->progress.x;

    if (peek->direction == UI_AUTO_LAYOUT_COLUMN)
    {
        peek->progress.y += rect->w;
        peek->progress.y += peek->gap;
    }
    else
    {
        peek->progress.x += rect->z;
        peek->progress.x += peek->gap;
    }
}

void UIPushAutoLayout(v2 pos, f32 gap, i32 direction)
{
    UIAutoLayout *peek = AutoLayoutStackPeek();

    assert(g_UI.autoLayoutCount < UI_AUTO_LAYOUT_STACK_MAX);
    UIAutoLayout *autoLayout = g_UI.autoLayoutStack + g_UI.autoLayoutCount++;

    v2 add = peek ? peek->progress : v2(0, 0);

    autoLayout->direction = direction;
    autoLayout->progress = v2(pos.x + add.x, pos.y + add.y);
    autoLayout->gap = gap;
}

void UIPopAutoLayout(void)
{
    if (g_UI.autoLayoutCount > 0)
    {
        g_UI.autoLayoutCount--;
    }
}

void DrawUI(m4 projection)
{
    BeginDraw(g_Renderer.whiteTexture, projection);
    for (i32 i = 0; i < g_UI.elementCount; ++i)
    {
        UIElement *element = g_UI.elements + i;

        if (element->flags & UI_ACTIVE_COLOR && element->id == g_UI.active)
            element->background = element->backgroundActive;
        else if (element->flags & UI_HOT_COLOR && element->id == g_UI.hot)
            element->background = element->backgroundHot;

        if (element->flags & UI_BACKGROUND_VISIBLE)
        {
            m4 transform = M4Translate(m4(1), v3(element->rect.x, element->rect.y, 1));
            transform = M4Scale(transform, v3(element->rect.z, element->rect.w, 1));

            *AllocateQuads(1) =
                CreateQuad(transform, v4(0, 0, 1, 1), element->background);
        }

        if (element->flags & UI_TEXT_CURSOR && element->id == g_UI.active &&
            fmodf(g_UI.time, 0.8f) < 0.4f)
        {
            enum
            {
                CURSOR_MARGIN_Y = 4,
            };

            m4 transform =
                M4Translate(m4(1), v3(element->rect.x +
                                          CalculateTextWidth(element->text, fnt_Primary),
                                      element->rect.y + CURSOR_MARGIN_Y, 0));
            transform =
                M4Scale(transform, v3(2, element->rect.w - (CURSOR_MARGIN_Y * 2), 1));
            *AllocateQuads(1) = CreateQuad(transform, v4(0, 0, 1, 1), v4(1, 1, 1, 1));
        }
    }
    EndDraw();

    BeginDraw(fnt_Primary.texture, projection);
    for (i32 i = 0; i < g_UI.elementCount; ++i)
    {
        UIElement *element = g_UI.elements + i;

        if (element->flags & UI_LIFT_WHEN_HOT && element->id == g_UI.hot)
            element->rect.y -= 2;

        if (element->flags & UI_ACTIVE_COLOR && element->id == g_UI.active)
            element->foreground = element->foregroundActive;
        else if (element->flags & UI_HOT_COLOR && element->id == g_UI.hot)
            element->foreground = element->foregroundHot;

        if (element->flags & UI_TEXT_VISIBLE)
            DrawFont(element->text, fnt_Primary, v2(element->rect.x, element->rect.y),
                     element->foreground);
    }
    EndDraw();
}

void UIInputText(UIID id, char *text, i32 bufferSize, v2 pos, i32 flags)
{
    UIElement *element =
        AllocateUIElement(id,
                          UI_TEXT_VISIBLE | UI_BACKGROUND_VISIBLE | UI_HOT_COLOR |
                              UI_ACTIVE_COLOR | UI_TEXT_CURSOR,
                          v4(pos.x, pos.y, 300, 48));

    if (pos.x == 0.0f && pos.y == 0.0f)
    {
        GetNextLayoutPosition(&element->rect);
    }

    element->text = text;

    element->foreground = v4(0.8f, 0.8f, 0.8f, 1);
    element->background = v4(0.1f, 0.1f, 0.1f, 1.0f);

    element->foregroundHot = element->foreground;
    element->backgroundHot = v4(0.2f, 0.2f, 0.2f, 1.0f);

    element->foregroundActive = v4(1, 1, 1, 1);
    element->backgroundActive = v4(0.2f, 0.2f, 0.2f, 1.0f);

    b32 hover = RectVersusV2(element->rect, MousePos());

    if (hover)
    {
        g_UI.hot = id;
    }
    else if (g_UI.hot == id)
    {
        g_UI.hot = 0;
    }

    if (MouseClicked(GLFW_MOUSE_BUTTON_LEFT))
    {
        if (g_UI.hot == id)
        {
            g_UI.active = id;
            g_UI.time = 0;
        }
        else if (g_UI.active == id)
        {
            g_UI.active = 0;
        }
    }

    if (g_UI.active == id)
    {
        i32 cursor = (i32)strlen(text);

        i32 keyCount;
        i32 *stack = GetKeyStack(&keyCount);

        for (i32 i = 0; i < keyCount; ++i)
        {
            if (IsKeyCharacter(stack[i]) && cursor < bufferSize - 1)
            {
                b32 valid = true;
                char ch = (char)stack[i];

                if (flags & UI_INPUT_NUMERIC && !('0' <= ch && ch <= '9'))
                {
                    valid = false;
                }

                if (flags & UI_INPUT_ALPHA && !('A' <= ch && ch <= 'Z'))
                {
                    valid = false;
                }

                if (valid)
                {
                    text[cursor++] = (char)stack[i];
                    g_UI.time = 0;
                }
            }
            else if (stack[i] == GLFW_KEY_BACKSPACE && cursor > 0)
            {
                cursor--;
                g_UI.time = 0;
            }
        }

        text[cursor] = 0;
    }
}

b32 UITextButton(UIID id, const char *text, v2 pos)
{
    UIElement *element = AllocateUIElement(
        id, UI_TEXT_VISIBLE | UI_LIFT_WHEN_HOT | UI_HOT_COLOR, v4(pos.x, pos.y, 150, 32));

    if (pos.x == 0.0f && pos.y == 0.0f)
    {
        GetNextLayoutPosition(&element->rect);
    }

    element->text = text;
    element->foreground = v4(0.8f, 0.8f, 0.8f, 1);
    element->foregroundHot = v4(1, 1, 1, 1);

    if (RectVersusV2(element->rect, MousePos()))
    {
        g_UI.hot = id;
    }
    else if (g_UI.hot == id)
    {
        g_UI.hot = 0;
    }

    if (g_UI.active == id && MouseReleased(GLFW_MOUSE_BUTTON_LEFT))
    {
        g_UI.active = 0;
        return g_UI.hot == id;
    }

    if (g_UI.hot == id && MouseClicked(GLFW_MOUSE_BUTTON_LEFT))
    {
        g_UI.active = id;
    }

    return false;
}
