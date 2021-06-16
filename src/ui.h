#pragma once

#define STB_TEXTEDIT_CHARTYPE char
#define STB_TEXTEDIT_POSITIONTYPE i32
#define STB_TEXTEDIT_UNDOSTATECOUNT 32
#define STB_TEXTEDIT_UNDOCHARCOUNT 256

#include "font.h"
#include "geometry.h"
#include "language.h"
#include <stb_textedit.h>

typedef u64 UIID;

#define UIID() ((UIID)__LINE__)
#define UIIDKeyed(key) ((UIID)(((key) << 32) | __LINE__))
#define UIIDPair(primary, secondary) ((UIID)((primary) << 32) | (secondary))

//

enum UIAutoLayoutDirection
{
    UI_AUTO_LAYOUT_COLUMN,
    UI_AUTO_LAYOUT_ROW,
};

enum UIInputTextFlags
{
    UI_INPUT_ANY = 0,
    UI_INPUT_NUMERIC = 1 << 0,
    UI_INPUT_ALPHA = 1 << 1,
};

enum UIKeyModFlags
{
    UI_KEY_SHIFT = 1 << 30,
    UI_KEY_CTRL = 1 << 29,
};

typedef struct UITextState UITextState;
struct UITextState
{
    char *cstr;
    i32 capacity;
    i32 length;
    Font font;
    STB_TexteditState stb;
};

UITextState CreateTextState(void);
i32 TextStateLength(UITextState *text);
void TextStateLayoutRow(StbTexteditRow *row, UITextState *text, i32 n);
f32 TextStateGetWidth(UITextState *text, i32 n, i32 i);
i32 KeyToChar(i32 key);
char TextStateCharAt(UITextState *text, i32 i);
void TextStateDelete(UITextState *text, i32 begin, i32 len);
b32 TextStateInsert(UITextState *text, i32 i, char *str, i32 strn);

void InitUI(void);

void UIBegin(f32 deltaTime);
void UIPushAutoLayout(v2 pos, f32 gap, i32 direction);
void UIPopAutoLayout(void);
void DrawUI(m4 projection);

void UIInputText(UIID id, char *text, i32 bufferSize, v2 pos, i32 flags);
b32 UITextButton(UIID id, const char *text, v2 pos);
void UILabel(UIID id, const char *text, v2 pos);
