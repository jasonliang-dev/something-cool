#pragma once

#define STB_TEXTEDIT_CHARTYPE char

#include "geometry.h"
#include "language.h"
// #include <stb_textedit.h>

typedef u64 UIID;

#define UIID() ((UIID)__LINE__)
#define UIIDKeyed(key) ((UIID)(((key) << 32) | __LINE__))
#define UIIDPair(primary, secondary) ((UIID)((primary) << 32) | (secondary))

//

enum
{
    UI_AUTO_LAYOUT_COLUMN,
    UI_AUTO_LAYOUT_ROW,
};

enum
{
    UI_INPUT_ANY = 0,
    UI_INPUT_NUMERIC = 1 << 0,
    UI_INPUT_ALPHA = 1 << 1,
};

// typedef struct UIText UIText;
// struct UIText
// {
//     char *cstr;
//     i32 capacity;
//     Font font;
// };
//
// UIText CreateText(char *str, i32 capacity);
// i32 TextLength(UIText *text);
// char TextCharAt(UIText *text, i32 i);

void InitUI(void);

void UIBegin(f32 deltaTime);
void UIPushAutoLayout(v2 pos, f32 gap, i32 direction);
void UIPopAutoLayout(void);
void DrawUI(m4 projection);

void UIInputText(UIID id, char *text, i32 bufferSize, v2 pos, i32 flags);
b32 UITextButton(UIID id, const char *text, v2 pos);
