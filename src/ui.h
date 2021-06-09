#pragma once

#include "geometry.h"
#include "language.h"

#define UI_ELEMENTS_MAX 128

typedef char CheckSizeOfVoidPtr[sizeof(void *) == 8 ? 1 : -1];

typedef u64 UIID;

#define UIID() ((UIID)__LINE__)
#define UIIDKeyed(key) ((UIID)(((key) << 32) | __LINE__))
#define UIIDPair(primary, secondary) ((UIID)((secondary) << 32) | (primary))

//

void InitUI(void);

void UIBegin(void);
void DrawUI(m4 projection);

b32 UITextButton(UIID id, const char *text, v2 pos);
