#ifndef VISUAL_HPP
#define VISUAL_HPP

#include <SDL2/SDL.h>

#include <vector>
#include <iostream>
#include <algorithm>

typedef  int32_t i32;
typedef uint32_t u32;

class Visual {

    private:
        const u32 WIDTH  = 800;
        const u32 HEIGHT = 800;

        u32 rect_side;

        SDL_Renderer* renderer;
        SDL_Window* window;

        std::vector<std::vector<u32>> grid;
        std::vector<std::pair<u32, u32>> path;

    public:
        Visual(const std::vector<std::vector<u32>>& m_Grid);
        ~Visual();
        
        void setPath(const std::vector<std::pair<u32, u32>>& m_Path);
        void draw();

};

#endif //VISUAL_HPP
