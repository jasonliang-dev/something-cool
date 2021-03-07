internal inline char *StringDup(const char *other)
{
    return strcpy(malloc(strlen(other) + 1), other);
}
