// Algorithm inspired by: https://csis.pace.edu/~benjamin/teaching/cs627/webfiles/Astar.pdf (p.7)

#ifndef A_STAR_HPP
#define A_STAR_HPP

#include <iostream>
#include <utility>
#include <set>
#include <vector>
#include <unordered_set>
#include <unordered_map>

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

u32 heuristic(const std::pair<u32, u32> &a, const std::pair<u32, u32> &b);

std::vector<std::pair<u32, u32>> retracePath(
        const std::pair<u32, u32> &current, 
        const std::unordered_map<std::pair<u32, u32>, 
        std::pair<u32, u32>, pair_hash> &parents);

std::vector<std::pair<u32, u32>> getAdjacentSquares(
        const std::pair<u32, u32> &current, 
        const std::vector<std::vector<u32>> &nodes);

std::vector<std::pair<u32, u32>> aStarPathfinding(
        const std::pair<u32, u32> &start, 
        const std::pair<u32, u32> &target, 
        const std::vector<std::vector<u32>> &nodes);

#endif //A_STAR_HPP
