#include "AStar.hpp"

u32 heuristic(const std::pair<u32, u32> &a, const std::pair<u32, u32> &b) {
    u32 dx = abs(static_cast<i32>(a.first)  - static_cast<i32>(b.first));
    u32 dy = abs(static_cast<i32>(a.second) - static_cast<i32>(b.second));

    return 10 * (dx + dy);
}

std::vector<std::pair<u32, u32>> retracePath(
        const std::pair<u32, u32> &current, 
        const std::unordered_map<std::pair<u32, u32>, std::pair<u32, u32>, pair_hash> &parents) 
{
    std::vector<std::pair<u32, u32>> path = {current};

    std::pair<u32, u32> parent = current;
    do {
        parent = parents.find(parent)->second; 
        path.push_back(parent);
    } while (parents.find(parent) != parents.end());

    return path;
}

std::vector<std::pair<u32, u32>> getAdjacentSquares(
        const std::pair<u32, u32> &current, 
        const std::vector<std::vector<u32>> &nodes) 
{
    std::vector<std::pair<u32, u32>> adjacentSquares;

    u32 f = current.first;
    u32 s = current.second;

    if (f > 0) adjacentSquares.push_back(std::pair<u32, u32>(f - 1, s));
    if (s > 0) adjacentSquares.push_back(std::pair<u32, u32>(f, s - 1));
    if (f < 9) adjacentSquares.push_back(std::pair<u32, u32>(f + 1, s));
    if (s < 9) adjacentSquares.push_back(std::pair<u32, u32>(f, s + 1));

    return adjacentSquares; 
}

std::vector<std::pair<u32, u32>> aStarPathfinding(
        const std::pair<u32, u32> &start, 
        const std::pair<u32, u32> &target, 
        const std::vector<std::vector<u32>> &nodes) 
{
    std::set<std::pair<u32, std::pair<u32, u32>>> openSet;
    std::unordered_set<std::pair<u32, u32>, pair_hash> closedSet;

    std::unordered_map<std::pair<u32, u32>, u32, pair_hash> gScore;
    std::unordered_map<std::pair<u32, u32>, u32, pair_hash> fScore;

    std::unordered_map<std::pair<u32, u32>, std::pair<u32, u32>, pair_hash> parents;

    fScore[start] = gScore[start] = heuristic(start, target); 

    openSet.insert(std::pair<u32, std::pair<u32, u32>>(fScore[start], start));

    while (!openSet.empty()) {
        std::pair<u32, u32> current = openSet.begin()->second;

        if (current == target) {
            return retracePath(current, parents);
        }

        openSet.erase(openSet.begin());
        closedSet.insert(current);

        std::vector<std::pair<u32, u32>> adjacentSquares = getAdjacentSquares(current, nodes);
        for (const std::pair<u32, u32> &square : adjacentSquares) {
            if (nodes[square.first][square.second] == 1 || closedSet.find(square) != closedSet.end()) {
                continue;
            } else if (fScore.find(square) == fScore.end()) {
                gScore[square] = gScore[current] + heuristic(current, square);
                fScore[square] = gScore[square] + heuristic(square, target);
                parents[square] = current;

                openSet.insert(std::pair<u32, std::pair<u32, u32>>(fScore[square], square));
            } else {
                u32 new_gScore = gScore[current] + heuristic(current, square);
                if (new_gScore < gScore[square]) {
                    openSet.erase(std::pair<u32, std::pair<u32, u32>>(fScore[square], square)); 

                    gScore[square] = new_gScore;
                    fScore[square] = gScore[square] + heuristic(square, target);
                    parents[square] = current;

                    openSet.insert(std::pair<u32, std::pair<u32, u32>>(fScore[square], square)); 
                }
            }
        }
    }

    throw std::runtime_error("Could not find valid path!");
}
