#pragma once

// tiny subset of c++20's std::span

template <typename T> struct Span
{
    typedef T &reference;
    typedef const T &const_reference;
    typedef T *iterator;
    typedef const T *const_iterator;
    typedef size_t size_type;

    T *m_Begin = nullptr;
    size_t m_Size = 0;

    Span(void) = default;

    Span(iterator begin, size_type size) : m_Begin(begin), m_Size(size)
    {
    }

    iterator begin(void)
    {
        return m_Begin;
    }

    const_iterator begin(void) const
    {
        return m_Begin;
    }

    const_iterator cbegin(void) const
    {
        return m_Begin;
    }

    iterator end(void)
    {
        return m_Begin + m_Size;
    }

    const_iterator end(void) const
    {
        return m_Begin + m_Size;
    }

    const_iterator cend(void) const
    {
        return m_Begin + m_Size;
    }

    reference operator[](size_type i)
    {
        return m_Begin[i];
    }

    const_reference operator[](size_type i) const
    {
        return m_Begin[i];
    }
};
