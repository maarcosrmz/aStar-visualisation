#include "Visual.hpp"

Visual::Visual(const std::vector<std::vector<u32>>& m_Grid)
{
    grid = m_Grid;

    rect_side = WIDTH / grid.size();

    window = SDL_CreateWindow(
        "A* Visualization", 
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        throw std::runtime_error("Could not create window!");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr) {
        throw std::runtime_error("Could not create renderer!");
    }
}

Visual::~Visual()
{
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;

    SDL_DestroyWindow(window);
    window = nullptr;
}

void Visual::setPath(const std::vector<std::pair<u32, u32>>& m_Path)
{
    path = m_Path;
    std::reverse(path.begin(), path.end());
}

void Visual::draw()
{
    if (path.size() == 0) {
        throw std::runtime_error("Path to be visualized is undefined!");
    }

    SDL_Rect rect;
    for (size_t i = 0; i < path.size(); i++) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for (size_t j = 0; j <= i; j++) {
            rect = {
                static_cast<i32>(rect_side * path[j].second),
                static_cast<i32>(rect_side * path[j].first),
                static_cast<i32>(rect_side),
                static_cast<i32>(rect_side)
            };

            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        rect = {
            static_cast<i32>(rect_side * path[path.size() - 1].second),
            static_cast<i32>(rect_side * path[path.size() - 1].first),
            static_cast<i32>(rect_side),
            static_cast<i32>(rect_side)
        };
        SDL_RenderFillRect(renderer, &rect);

        for (size_t y = 0; y < grid.size(); y++) {
            for (size_t x = 0; x < grid[y].size(); x++) {
                rect = {
                    static_cast<i32>(rect_side * x),
                    static_cast<i32>(rect_side * y),
                    static_cast<i32>(rect_side),
                    static_cast<i32>(rect_side)
                };

                if (grid[y][x] == 1) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &rect);
                }

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &rect);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(500);
    }
}
