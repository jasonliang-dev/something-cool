typedef struct String String;
struct String
{
    u64 allocd;
    u64 length;
    char *ptr;
};
