typedef union v2 v2;
union v2
{
    struct
    {
        f32 x;
        f32 y;
    };

    struct
    {
        f32 width;
        f32 height;
    };

    float elements[2];
};

typedef union v3 v3;
union v3
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
    };

    struct
    {
        v2 xy;
        f32 ignore0;
    };

    struct
    {
        f32 ignore1;
        v2 yz;
    };

    struct
    {
        f32 r;
        f32 g;
        f32 b;
    };

    f32 elements[3];
};

typedef union v4 v4;
union v4
{
    struct
    {
        union
        {
            v3 xyz;
            struct
            {
                f32 x;
                f32 y;
                f32 z;
            };
        };

        f32 w;
    };

    struct
    {
        union
        {
            v3 rgb;
            struct
            {
                f32 r;
                f32 g;
                f32 b;
            };
        };

        f32 a;
    };

    struct
    {
        f32 ignore0;
        v2 yz;
        f32 ignore1;
    };

    struct
    {
        v2 xy;
        union
        {
            v2 zw;
            v2 wh;
            struct
            {
                f32 width;
                f32 height;
            };
        };
    };

    f32 elements[4];
};

typedef union m4 m4;
union m4
{
    f32 elements[4][4];
    f32 flatten[16];
};

#define v2(...)                                                                                    \
    (v2)                                                                                           \
    {                                                                                              \
        __VA_ARGS__                                                                                \
    }
#define v3(...)                                                                                    \
    (v3)                                                                                           \
    {                                                                                              \
        __VA_ARGS__                                                                                \
    }
#define v4(...)                                                                                    \
    (v4)                                                                                           \
    {                                                                                              \
        __VA_ARGS__                                                                                \
    }
