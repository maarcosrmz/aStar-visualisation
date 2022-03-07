#ifndef VISUALIZATION_HPP
#define VISUALIZATION_HPP

#include <SDL2/SDL.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"

#include "Util.hpp"
#include "AStar.hpp"
#include "EditStack.hpp"

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

        std::pair<std::vector<std::pair<i32, i32>>, short> stack_pair;
        EditStack edit_stack;

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
        void AboutMenu();

        // Events
        void OnMouseButtonDown(const SDL_Event& e);
        void OnMouseButtonUp(const SDL_Event& e);
        void OnMouseMotion(const SDL_Event& e);
        void OnKeyDown(const SDL_Event& e);

        // Undo / Redo Operations Methods 
        void OnUndo();
        void OnRedo();

        // Draw 
        void DrawGrid();

    public:
        Visualization();
        ~Visualization();
        
        void run();

};

#endif //VISUALIZATION_HPP
