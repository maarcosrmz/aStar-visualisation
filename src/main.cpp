#include "AStar.hpp"
#include "Visual.hpp"

int main() {
    std::pair<u32, u32> start(4, 2), target(4, 8);

    std::vector<std::vector<u32>> nodes = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    try {
        Visual visual(nodes);

        std::cout << "Finding shortest path...\n" << std::endl;
        auto path = aStarPathfinding(start, target, nodes);

        std::cout << "Visualizing...\n" << std::endl;
        visual.setPath(path);
        visual.draw();

        std::cout << "Press ENTER to end the program..." << std::endl;
        std::cin.get();

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
