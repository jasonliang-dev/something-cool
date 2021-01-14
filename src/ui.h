#define UI_MAX_WIDGETS 128
#define UI_MAX_FLEX_GROUP 16

#define UI_AutoID() UI_MakeID(__LINE__, 0)

enum ui_widget_type_t
{
    UI_WIDGET_BUTTON,
    UI_WIDGET_SLIDER,
};

enum ui_flex_direction_t
{
    UI_FLEX_ROW,
    UI_FLEX_COLUMN,
};

struct ui_id_t
{
    u32 primary;
    u32 secondary;
};

struct ui_widget_t
{
    ui_widget_type_t type;
    ui_id_t id;
    v4 box;
    f32 tHot;
    f32 tActive;

    union
    {
        struct
        {
            char *text;
        } button;

        struct
        {
            f32 value;
        } slider;
    };
};

struct ui_flex_stack_t
{
    ui_flex_direction_t direction;
    v2 position;
    v2 size;
    f32 progress;
};

struct ui_t
{

    u32 widgetCount;
    ui_widget_t widgets[UI_MAX_WIDGETS];

    u32 flexStackCount;
    ui_flex_stack_t flexStack[UI_MAX_FLEX_GROUP];

    ui_id_t hot;
    ui_id_t active;
};
