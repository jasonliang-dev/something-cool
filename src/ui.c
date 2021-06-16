#include "ui.h"
#include "assets.h"
#include "font.h"
#include "input.h"
#include "memory.h"
#include "renderer.h"
#include "window.h"
#include <assert.h>
#include <math.h>
#include <string.h>

#define STB_TEXTEDIT_STRING UITextState
#define STB_TEXTEDIT_STRINGLEN(obj) TextStateLength(obj)
#define STB_TEXTEDIT_LAYOUTROW(r, obj, n) TextStateLayoutRow(r, obj, n)
#define STB_TEXTEDIT_GETWIDTH(obj, n, i) TextStateGetWidth(obj, n, i)
#define STB_TEXTEDIT_KEYTOTEXT(k) KeyToChar(k)
#define STB_TEXTEDIT_GETCHAR(obj, i) TextStateCharAt(obj, i)
#define STB_TEXTEDIT_NEWLINE '\n'
#define STB_TEXTEDIT_DELETECHARS(obj, i, n) TextStateDelete(obj, i, n)
#define STB_TEXTEDIT_INSERTCHARS(obj, i, c, n) TextStateInsert(obj, i, c, n)
#define STB_TEXTEDIT_K_SHIFT UI_KEY_SHIFT
#define STB_TEXTEDIT_K_LEFT GLFW_KEY_LEFT
#define STB_TEXTEDIT_K_RIGHT GLFW_KEY_RIGHT
#define STB_TEXTEDIT_K_UP GLFW_KEY_UP
#define STB_TEXTEDIT_K_DOWN GLFW_KEY_DOWN
#define STB_TEXTEDIT_K_LINESTART GLFW_KEY_HOME
#define STB_TEXTEDIT_K_LINEEND GLFW_KEY_END
#define STB_TEXTEDIT_K_TEXTSTART (UI_KEY_CTRL | GLFW_KEY_HOME)
#define STB_TEXTEDIT_K_TEXTEND (UI_KEY_CTRL | GLFW_KEY_END)
#define STB_TEXTEDIT_K_DELETE GLFW_KEY_DELETE
#define STB_TEXTEDIT_K_BACKSPACE GLFW_KEY_BACKSPACE
#define STB_TEXTEDIT_K_UNDO (UI_KEY_CTRL | GLFW_KEY_Z)
#define STB_TEXTEDIT_K_REDO (UI_KEY_CTRL | GLFW_KEY_Y)
#define STB_TEXTEDIT_IS_SPACE(ch) isspace(ch)
#define STB_TEXTEDIT_K_WORDLEFT (UI_KEY_CTRL | GLFW_KEY_LEFT)
#define STB_TEXTEDIT_K_WORDRIGHT (UI_KEY_CTRL | GLFW_KEY_RIGHT)

#define STB_TEXTEDIT_IMPLEMENTATION
#include <stb_textedit.h>

#define UI_ELEMENTS_MAX 128
#define UI_AUTO_LAYOUT_STACK_MAX 16

enum UIElementFlags
{
    UI_TEXT_VISIBLE = 1 << 0,
    UI_LIFT_WHEN_HOT = 1 << 1,
    UI_HOT_COLOR = 1 << 2,
    UI_ACTIVE_COLOR = 1 << 3,
    UI_BACKGROUND_VISIBLE = 1 << 4,
    UI_TEXT_INPUT = 1 << 5,
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
    UITextState textState;

    UIAutoLayout autoLayoutStack[UI_AUTO_LAYOUT_STACK_MAX];
    i32 autoLayoutCount;

    UIElement elements[UI_ELEMENTS_MAX];
    i32 elementCount;
} g_UI;

UITextState CreateTextState(void)
{
    UITextState result;

    result.cstr = NULL;
    result.capacity = 0;
    result.length = 0;
    result.font = fnt_Primary;
    stb_textedit_initialize_state(&result.stb, true);

    return result;
}

i32 TextStateLength(UITextState *text)
{
    return text->length;
}

void TextStateLayoutRow(StbTexteditRow *row, UITextState *text, i32 n)
{
    row->x0 = CalculateTextWidth(text->cstr, n, text->font);
    row->x1 = CalculateTextWidth(text->cstr + n, text->length - n, text->font);
    row->baseline_y_delta = 1.0f;
    row->ymin = -1.0f;
    row->ymax = 0.0f;
    row->num_chars = text->length - n;
}

f32 TextStateGetWidth(UITextState *text, i32 n, i32 i)
{
    stbtt_packedchar *packed =
        text->font.characters + text->cstr[n + i] - FONT_FIRST_CHARACTER;
    return packed->xadvance;
}

i32 KeyToChar(i32 key)
{
    i32 noFlags = key;
    noFlags &= ~UI_KEY_SHIFT;
    noFlags &= ~UI_KEY_CTRL;

    if (!IsKeyCharacter(noFlags))
    {
        return -1;
    }

    if (isalpha(noFlags))
    {
        if (key & UI_KEY_SHIFT)
        {
            return noFlags;
        }
        return tolower(noFlags);
    }
    else if ('0' <= noFlags && noFlags <= '9')
    {
        if (key & UI_KEY_SHIFT)
        {
            const char *table = ")!@#$%^&*(";
            return table[noFlags - '0'];
        }

        return noFlags;
    }
    else
    {
        if (key & UI_KEY_SHIFT)
        {
            switch (noFlags)
            {
            case '`':
                return '~';
            case '-':
                return '_';
            case '=':
                return '+';
            case '[':
                return '{';
            case ']':
                return '}';
            case '\\':
                return '|';
            case ';':
                return ':';
            case '\'':
                return '"';
            case ',':
                return '<';
            case '.':
                return '>';
            case '/':
                return '?';
            }
        }

        // good enough.
        return noFlags;
    }
}

char TextStateCharAt(UITextState *text, i32 i)
{
    return text->cstr[i];
}

void TextStateDelete(UITextState *text, i32 pos, i32 len)
{
    char *begin = text->cstr + pos;
    memmove(begin, begin + len, text->length - (pos + len));
    text->length -= len;
    text->cstr[text->length] = 0;
}

b32 TextStateInsert(UITextState *text, i32 pos, char *str, i32 strn)
{
    if (text->length + strn >= text->capacity)
    {
        return false;
    }

    char *begin = text->cstr + pos;
    memmove(begin + strn, begin, text->length - pos);
    memcpy(begin, str, strn);
    text->length += strn;
    text->cstr[text->length] = 0;

    return true;
}

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
    g_UI.textState = CreateTextState();
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
    BeginDraw(projection);
    for (i32 i = 0; i < g_UI.elementCount; ++i)
    {
        UIElement *element = g_UI.elements + i;

        if (element->flags & UI_ACTIVE_COLOR && element->id == g_UI.active)
        {
            element->background = element->backgroundActive;
            element->foreground = element->foregroundActive;
        }
        else if (element->flags & UI_HOT_COLOR && element->id == g_UI.hot)
        {
            element->background = element->backgroundHot;
            element->foreground = element->foregroundHot;
        }

        if (element->flags & UI_LIFT_WHEN_HOT && element->id == g_UI.hot)
        {
            element->rect.y -= 2;
        }

        if (element->flags & UI_BACKGROUND_VISIBLE)
        {
            m4 transform = M4Translate(m4(1), v3(element->rect.x, element->rect.y, 1));
            transform = M4Scale(transform, v3(element->rect.z, element->rect.w, 1));

            DrawQuad(transform, v4(0, 0, 1, 1), tex_White.id, element->background);
        }

        if (element->flags & UI_TEXT_INPUT && element->id == g_UI.active)
        {
            enum
            {
                CURSOR_MARGIN_Y = 4,
            };

            { // selection region
                i32 min;
                i32 max;
                if (g_UI.textState.stb.select_start < g_UI.textState.stb.select_end)
                {
                    min = g_UI.textState.stb.select_start;
                    max = g_UI.textState.stb.select_end;
                }
                else
                {
                    min = g_UI.textState.stb.select_end;
                    max = g_UI.textState.stb.select_start;
                }

                m4 transform = M4Translate(
                    m4(1), v3(element->rect.x +
                                  CalculateTextWidth(element->text, min, fnt_Primary),
                              element->rect.y + CURSOR_MARGIN_Y, 0));
                transform = M4Scale(
                    transform,
                    v3(CalculateTextWidth(element->text + min, max - min, fnt_Primary),
                       element->rect.w - (CURSOR_MARGIN_Y * 2), 1));

                DrawQuad(transform, v4(0, 0, 1, 1), tex_White.id,
                         v4(0.4f, 0.4f, 0.6f, 0.4f));
            }

            { // cursor
                m4 transform = M4Translate(
                    m4(1),
                    v3(element->rect.x + CalculateTextWidth(element->text,
                                                            g_UI.textState.stb.cursor,
                                                            fnt_Primary),
                       element->rect.y + CURSOR_MARGIN_Y, 0));
                transform =
                    M4Scale(transform, v3(2, element->rect.w - (CURSOR_MARGIN_Y * 2), 1));

                DrawQuad(transform, v4(0, 0, 1, 1), tex_White.id, v4(1, 1, 1, 1));
            }
        }

        if (element->flags & UI_TEXT_VISIBLE)
        {
            DrawFont(element->text, fnt_Primary, v2(element->rect.x, element->rect.y),
                     element->foreground);
        }
    }
    EndDraw();
}

static void HandleInputTextKeyPress(UITextState *textState, i32 mods, i32 key, i32 flags)
{
    if (mods & UI_KEY_CTRL && (key == GLFW_KEY_X || key == GLFW_KEY_C))
    {
        i32 min;
        i32 max;
        if (textState->stb.select_start < textState->stb.select_end)
        {
            min = textState->stb.select_start;
            max = textState->stb.select_end;
        }
        else
        {
            min = textState->stb.select_end;
            max = textState->stb.select_start;
        }

        char *buf = ScratchAlloc(max - min);
        memcpy(buf, textState->cstr + min, max - min);
        buf[max - min] = 0;
        glfwSetClipboardString(g_Window.handle, buf);

        if (key == GLFW_KEY_X)
        {
            stb_textedit_cut(textState, &textState->stb);
        }

        return;
    }

    if (mods & UI_KEY_CTRL && key == GLFW_KEY_V)
    {
        const char *clipboard = glfwGetClipboardString(g_Window.handle);
        stb_textedit_paste(textState, &textState->stb, clipboard, (i32)strlen(clipboard));

        return;
    }

    if (IsKeyCharacter(key))
    {
        if (flags & UI_INPUT_NUMERIC)
        {
            if (!isdigit(key))
            {
                return;
            }
            else if (mods & UI_KEY_SHIFT)
            {
                return;
            }
        }

        if (flags & UI_INPUT_ALPHA && !isalpha(key))
        {
            return;
        }
    }

    stb_textedit_key(textState, &textState->stb, mods | key);
}

void UIInputText(UIID id, char *text, i32 bufferSize, v2 pos, i32 flags)
{
    UIElement *element =
        AllocateUIElement(id,
                          UI_TEXT_VISIBLE | UI_BACKGROUND_VISIBLE | UI_HOT_COLOR |
                              UI_ACTIVE_COLOR | UI_TEXT_INPUT,
                          v4(pos.x, pos.y, 300, 42));

    if (pos.x == 0.0f && pos.y == 0.0f)
    {
        GetNextLayoutPosition(&element->rect);
    }

    element->text = text;

    element->foreground = v4(0.8f, 0.8f, 0.8f, 1);
    element->background = v4(0.1f, 0.1f, 0.1f, 1.0f);

    element->foregroundHot = element->foreground;
    element->backgroundHot = v4(0.15f, 0.15f, 0.15f, 1.0f);

    element->foregroundActive = v4(1, 1, 1, 1);
    element->backgroundActive = v4(0.15f, 0.15f, 0.15f, 1.0f);

    v2 mousePos = MousePos();
    b32 hover = RectVersusV2(element->rect, mousePos);

    if (hover)
    {
        g_UI.hot = id;
    }
    else if (g_UI.hot == id)
    {
        g_UI.hot = 0;
    }

    UITextState *textState = &g_UI.textState;

    if (MouseClicked(GLFW_MOUSE_BUTTON_LEFT))
    {
        if (g_UI.hot == id)
        {
            if (g_UI.active != id)
            {
                g_UI.active = id;
                stb_textedit_initialize_state(&textState->stb, true);

                textState->cstr = text;
                textState->capacity = bufferSize;
                textState->length = (i32)strlen(text);
            }

            g_UI.time = 0;
            stb_textedit_click(textState, &textState->stb, mousePos.x - element->rect.x,
                               0);
        }
        else if (g_UI.active == id)
        {
            g_UI.active = 0;
        }
    }
    else if (MouseDown(GLFW_MOUSE_BUTTON_LEFT))
    {
        if (g_UI.active == id)
        {
            stb_textedit_drag(textState, &textState->stb, mousePos.x - element->rect.x,
                              0);
        }
    }

    if (g_UI.active == id)
    {
        i32 mods = 0;

        if (KeyDown(GLFW_KEY_LEFT_SHIFT) || KeyDown(GLFW_KEY_RIGHT_SHIFT))
        {
            mods |= UI_KEY_SHIFT;
        }

        if (KeyDown(GLFW_KEY_LEFT_CONTROL) || KeyDown(GLFW_KEY_RIGHT_CONTROL))
        {
            mods |= UI_KEY_CTRL;
        }

        KeyStack stack = GetKeyStack();
        for (i32 *it = stack.begin; it != stack.end; ++it)
        {
            HandleInputTextKeyPress(textState, mods, *it, flags);
        }
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

void UILabel(UIID id, const char *text, v2 pos)
{
    UIElement *element =
        AllocateUIElement(id, UI_TEXT_VISIBLE, v4(pos.x, pos.y, 150, 32));

    if (pos.x == 0.0f && pos.y == 0.0f)
    {
        GetNextLayoutPosition(&element->rect);
    }

    element->text = text;
    element->foreground = v4(0.6f, 0.6f, 0.6f, 1);
}
