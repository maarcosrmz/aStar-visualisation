#ifndef VISUALIZATION_HPP
#define VISUALIZATION_HPP

#include <SDL2/SDL.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"

#include "Util.hpp"
#include "AStar.hpp"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

class Visualization {

    private:
        // Attributes
        const u32 WIDTH  = 1280;
        const u32 HEIGHT =  720;

        bool running = true;
        bool menu_open = false;

        SDL_Renderer* renderer;
        SDL_Window* window;

        AStar aStar;

        i32 menu_bar_height;
        ImVec4 background_color;

        // Init
        void InitSdl();
        void InitImGui();

        // Methods
        void Input();
        void Update();
        void Render();

        // Menu Bar
        void MenuBar();
        void ResizeWindow();
        void FileMenu();
        void EditMenu();
        void RunMenu();
        void GridMenu();
        void ColorMenu();

        // Events
        void OnMouseButtonDown(const SDL_Event& e);
        void OnMouseButtonUp(const SDL_Event& e);
        void OnMouseMotion(const SDL_Event& e);

        // Draw 
        void DrawGrid();

    public:
        Visualization();
        ~Visualization();
        
        void run();

};

#endif //VISUALIZATION_HPP
