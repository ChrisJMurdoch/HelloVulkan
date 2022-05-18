
#pragma once

#include <vector>

namespace util
{
    template<class T>
    size_t vecsizeof(std::vector<T> vector)
    {
        return vector.size() * sizeof(T);
    }
};
