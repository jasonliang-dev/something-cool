#pragma once

#include "geometry.h"
#include "language.h"

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

void InitUI(void);

void UIBegin(f32 deltaTime);
void UIPushAutoLayout(v2 pos, f32 gap, i32 direction);
void UIPopAutoLayout(void);
void DrawUI(m4 projection);

void UIInputText(UIID id, char *text, i32 bufferSize, v2 pos, i32 flags);
b32 UITextButton(UIID id, const char *text, v2 pos);
