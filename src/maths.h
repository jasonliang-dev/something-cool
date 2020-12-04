typedef union v2 v2;
union v2 {
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
union v3 {
    struct
    {
        f32 x;
        f32 y;
        f32 z;
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
union v4 {
    struct
    {
        f32 x;
        f32 y;
        union {
            struct
            {
                f32 z;
                f32 w;
            };
            struct
            {
                f32 width;
                f32 height;
            };
        };
    };

    struct
    {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };

    f32 elements[4];
};

typedef union iv2 iv2;
union iv2 {
    struct
    {
        i32 x;
        i32 y;
    };

    struct
    {
        i32 width;
        i32 height;
    };

    i32 elements[2];
};

typedef union iv3 iv3;
union iv3 {
    struct
    {
        i32 x;
        i32 y;
        i32 z;
    };

    struct
    {
        i32 r;
        i32 g;
        i32 b;
    };

    i32 elements[3];
};

typedef union iv4 iv4;
union iv4 {
    struct
    {
        i32 x;
        i32 y;
        union {
            struct
            {
                i32 z;
                i32 w;
            };
            struct
            {
                i32 width;
                i32 height;
            };
        };
    };

    struct
    {
        i32 r;
        i32 g;
        i32 b;
        i32 a;
    };

    i32 elements[4];
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
#define iv2(...)                                                                                   \
    (iv2)                                                                                          \
    {                                                                                              \
        __VA_ARGS__                                                                                \
    }
#define iv3(...)                                                                                   \
    (iv3)                                                                                          \
    {                                                                                              \
        __VA_ARGS__                                                                                \
    }
#define iv4(...)                                                                                   \
    (iv4)                                                                                          \
    {                                                                                              \
        __VA_ARGS__                                                                                \
    }
