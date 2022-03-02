#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <utility>
#include <set>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <thread>

typedef  int32_t i32;
typedef uint32_t u32;

struct pair_hash {
    template <class T1, class T2>
        std::size_t operator () (std::pair<T1, T2> const &pair) const {
            std::size_t h1 = std::hash<T1>()(pair.first);
            std::size_t h2 = std::hash<T2>()(pair.second);

            return h1 ^ h2;
        }
};

#endif //UTIL_HPP
