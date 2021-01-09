#define UI_MAX_WIDGETS 128
#define UI_MAX_FLEX_GROUP 16

#define UI_AutoID() UI_MakeID(__LINE__, 0)

typedef enum ui_widget_type_t ui_widget_type_t;
enum ui_widget_type_t
{
    UI_WIDGET_BUTTON,
    UI_WIDGET_SLIDER,
};

typedef enum ui_flex_direction_t ui_flex_direction_t;
enum ui_flex_direction_t
{
    UI_FLEX_ROW,
    UI_FLEX_COLUMN,
};

typedef struct ui_id_t ui_id_t;
struct ui_id_t
{
    u32 primary;
    u32 secondary;
};

typedef struct ui_widget_t ui_widget_t;
struct ui_widget_t
{
    ui_widget_type_t type;
    ui_id_t id;
    v4 box;
    f32 tHot;
    f32 tActive;

    union
    {
        struct ui_slider_t
        {
            f32 value;
        } slider;
    };
};

typedef struct ui_input_t ui_input_t;
struct ui_input_t
{
    v2 cursor;
    b32 leftDown;
    b32 rightDown;
};

typedef struct ui_flex_stack_t ui_flex_stack_t;
struct ui_flex_stack_t
{
    ui_flex_direction_t direction;
    v2 position;
    v2 size;
    f32 progress;
};

typedef struct ui_t ui_t;
struct ui_t
{
    v2 cursor;
    b32 leftDown;
    b32 rightDown;

    u32 widgetCount;
    ui_widget_t widgets[UI_MAX_WIDGETS];

    u32 flexStackCount;
    ui_flex_stack_t flexStack[UI_MAX_FLEX_GROUP];

    ui_id_t hot;
    ui_id_t active;
};
