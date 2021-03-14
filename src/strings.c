internal String CreateString(const char *cstr)
{
    String result;
    result.length = strlen(cstr);
    result.allocd = result.length * 2;

    result.ptr = MemAlloc(result.allocd);
    memcpy(result.ptr, cstr, result.length);
    result.ptr[result.length] = 0;

    return result;
}

internal String CreateStringReserve(u64 n)
{
    String result;
    result.length = 0;
    result.allocd = n;

    result.ptr = MemAlloc(result.allocd);
    result.ptr[0] = 0;

    return result;
}

internal void FreeString(String str)
{
    free(str.ptr);
}

internal char *StringCat(String *str, const char *other)
{
    size_t otherLength = strlen(other);

    if (str->allocd < str->length + otherLength)
    {
        str->allocd = Max(str->allocd * 2, str->length + otherLength);
        str->ptr = MemRealloc(str->ptr, str->allocd);
    }

    memcpy(str->ptr + str->length, other, otherLength);
    str->length += otherLength;
    str->ptr[str->length] = 0;

    return str->ptr;
}

internal char *StringRem(String *str, u64 n)
{
    if (n > str->length)
    {
        n = str->length;
    }

    str->length -= n;
    str->ptr[str->length] = 0;
    return str->ptr;
}
