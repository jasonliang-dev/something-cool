#pragma once

#include <cstddef>

// very very small subset of c++20's std::span

template <typename T> struct Span
{
    Span(void) = default;

    Span(T *begin, size_t size) noexcept : m_Begin(begin), m_Size(size)
    {
    }

    T *begin(void) const noexcept
    {
        return m_Begin;
    }

    T *end(void) const noexcept
    {
        return m_Begin + m_Size;
    }

    T &operator[](size_t i) const
    {
        return m_Begin[i];
    }

    T *m_Begin = nullptr;
    size_t m_Size = 0;
};
