#include <stdexcept>
#include <cstdlib>
#include <iostream>

#include "Visualization.hpp"

int main() 
{
    try {
        Visualization app;
        app.run();

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
