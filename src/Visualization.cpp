#include "Visualization.hpp"

Visualization::Visualization()
{
    aStar = AStar({0, 0}, {BASE_WIDTH - 1, BASE_HEIGHT - 1});
    aStar.setDimensions({BASE_WIDTH, BASE_HEIGHT});
    aStar.calcDeltaLength(WIDTH, HEIGHT);
    aStar.setSelected(NONE);
    aStar.setDelay(50);

    menu_bar_height = 0;

    InitSdl();
    InitImGui();
}

Visualization::~Visualization()
{
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    renderer = nullptr;

    SDL_DestroyWindow(window);
    window = nullptr;

    SDL_Quit();
}

void Visualization::InitSdl()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) < 0) {
        throw std::runtime_error("Couldn't initialize SDL!");
    }

    window = SDL_CreateWindow(
        "A* Visualizationization", 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (window == nullptr) {
        throw std::runtime_error("Could not create window!");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr) {
        throw std::runtime_error("Could not create renderer!");
    }
}

void Visualization::InitImGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);
}

// NOTE: Add Keyboard Shortcuts
void Visualization::Input()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {

        ImGui_ImplSDL2_ProcessEvent(&e);
        switch (e.type) {
            
            case SDL_QUIT:
                running = false;
                break;
            
            case SDL_MOUSEBUTTONDOWN:
                OnMouseButtonDown(e);
                break;

            case SDL_MOUSEBUTTONUP:
                OnMouseButtonUp(e);
                break;

            case SDL_MOUSEMOTION: 
                OnMouseMotion(e); 
                break;

            default:
                break;
        };
    }
}

void Visualization::Update()
{
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    MenuBar();
}

void Visualization::Render()
{
    ImGui::Render();
    SDL_SetRenderDrawColor(renderer, 
            (Uint8)(background_color.x * 255), 
            (Uint8)(background_color.y * 255), 
            (Uint8)(background_color.z * 255), 
            (Uint8)(background_color.w * 255));
    SDL_RenderClear(renderer);

    DrawGrid();

    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
}

void Visualization::MenuBar()
{
    menu_open = false;
    if (ImGui::BeginMainMenuBar()) {
        ResizeWindow();
        FileMenu();
        EditMenu();
        RunMenu();
        GridMenu();
        ColorMenu();
        ImGui::EndMainMenuBar();
    }
}

void Visualization::ResizeWindow()
{
    int win_h;
    SDL_GetWindowSize(window, nullptr, &win_h);

    menu_bar_height = ImGui::GetWindowSize().y;
    if (win_h != menu_bar_height + HEIGHT) {
        SDL_SetWindowSize(window, WIDTH, menu_bar_height + HEIGHT);
    }
}

void Visualization::FileMenu() 
{
    if (ImGui::BeginMenu("File")) {
        menu_open = true;

        if (ImGui::MenuItem("New", "Ctrl+N")) {}
        if (ImGui::MenuItem("Open", "Ctrl+O")) {}
        if (ImGui::MenuItem("Save", "Ctrl+S")) {}
        if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {}

        ImGui::Separator();

        if (ImGui::MenuItem("Quit", "Alt+F4")) { running = false; }

        ImGui::EndMenu();
    }
}

void Visualization::EditMenu() 
{
    if (ImGui::BeginMenu("Edit")) {
        menu_open = true;

        if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
        if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}

        ImGui::EndMenu();
    }
}

void Visualization::RunMenu() 
{
    if (ImGui::BeginMenu("Run")) {
        menu_open = true;
        
        if (ImGui::MenuItem("Run", "Ctrl+R")) {
            aStar.startSimulation();
        }

        std::string label;
        if (aStar.getState() == SIMULATING) {
            label = "Abort";
        } else {
            label = "Finish";
        }

        if (ImGui::MenuItem(label.c_str(), "Ctrl+A")) {
            if (aStar.getState() == SIMULATING) {
                aStar.setState(FINISHED);
            } else {
                aStar.setState(EDITING);
            }
        }

        ImGui::Separator();

        i32 delay = aStar.getDelay();
        ImGui::SliderInt("Delay (ms)", &delay, 0, 100);
        aStar.setDelay(delay);

        ImGui::EndMenu();
    }
}

void Visualization::GridMenu() 
{
    if (ImGui::BeginMenu("Grid")) {
        menu_open = true;

        auto dims = aStar.getDimensions();
        ImGui::Text("Dimensions: %dx%d", dims.first, dims.second);

        ImGui::Separator();

        if (ImGui::BeginMenu("Scaling Factor")) {
            int scalar = aStar.getScalar();
            ImGui::RadioButton(" x1", &scalar,  1);
            ImGui::RadioButton(" x2", &scalar,  2);
            ImGui::RadioButton(" x4", &scalar,  4);
            ImGui::RadioButton(" x5", &scalar,  5);
            ImGui::RadioButton(" x8", &scalar,  8);
            ImGui::RadioButton("x10", &scalar, 10);

            aStar.setScalar(scalar);
            aStar.calcDeltaLength(WIDTH, HEIGHT);

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Clear Obstacles", "Ctrl+D")) {
            aStar.clearObstacles();
        }

        bool show = aStar.gridIsShown();
        ImGui::Checkbox("Show", &show);
        if (show) aStar.showGrid();
        else aStar.hideGrid();

        ImGui::EndMenu();
    }
}

void Visualization::ColorMenu() 
{
    if (ImGui::BeginMenu("Color")) {
        menu_open = true;

        ImVec4 color = aStar.getStartColor();
        ImGui::ColorEdit3("Start", (float*)&color);
        aStar.setStartColor(color);

        color = aStar.getTargetColor();
        ImGui::ColorEdit3("Target", (float*)&color);
        aStar.setTargetColor(color);

        color = aStar.getObstacleColor();
        ImGui::ColorEdit3("Obstacles", (float*)&color);
        aStar.setObstacleColor(color);

        color = aStar.getGridColor();
        ImGui::ColorEdit3("Grid", (float*)&color);
        aStar.setGridColor(color);

        ImGui::ColorEdit3("Background", (float*)&background_color);

        ImGui::EndMenu();
    }
}

void Visualization::OnMouseButtonDown(const SDL_Event& e)
{
    if (!menu_open 
            && aStar.getState() == EDITING
            && aStar.getSelected() == NONE 
            && e.button.button == SDL_BUTTON_LEFT) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        
        auto mouse_pos = aStar.mouseGetOver(x, y - menu_bar_height);
        auto obstacles = aStar.getObstacles();

        if (mouse_pos == aStar.getTarget()) {
            aStar.setSelected(TARGET);
        } else if (mouse_pos == aStar.getStart()) {
            aStar.setSelected(START);
        } else if (obstacles.find(mouse_pos) != obstacles.end()) {
            aStar.setSelected(OBSTACLE);
            aStar.removeObstacle(mouse_pos);
        } else {
            aStar.setSelected(BLANCK);
            aStar.addObstacle(mouse_pos);
        }
    }
}

void Visualization::OnMouseButtonUp(const SDL_Event& e)
{
    if (aStar.getSelected() != NONE && e.button.button == SDL_BUTTON_LEFT) {
        aStar.setSelected(NONE);
    }
}

void Visualization::OnMouseMotion(const SDL_Event& e)
{
    auto mouse_pos = aStar.mouseGetOver(
            e.motion.x, 
            e.motion.y - menu_bar_height);

    if (!menu_open 
            && aStar.getState() == EDITING
            && !aStar.mouseOutOfBounds(mouse_pos)) {
        bool mouse_on_other_tile = aStar.mouseOnOtherTile(mouse_pos, aStar.getSelected());

        if (aStar.getSelected() == START && !mouse_on_other_tile) {
            aStar.setStart(mouse_pos);
        } else if (aStar.getSelected() == TARGET && !mouse_on_other_tile) {
            aStar.setTarget(mouse_pos);
        } else if (aStar.getSelected() == OBSTACLE && !mouse_on_other_tile) {
            aStar.removeObstacle(mouse_pos);
        } else if (aStar.getSelected() == BLANCK && !mouse_on_other_tile) {
            aStar.addObstacle(mouse_pos);
        }
    }
}

void Visualization::DrawGrid()
{
    ImVec4 color;
    SDL_Rect rect;

    i32 dl = (i32)aStar.getDeltaLength();

    // Draw Obstacles
    color = aStar.getObstacleColor();
    SDL_SetRenderDrawColor(renderer, 
            (Uint8)(color.x * 255), 
            (Uint8)(color.y * 255), 
            (Uint8)(color.z * 255), 
            (Uint8)(color.w * 255));

    auto obstacles = aStar.getObstacles();
    for (const auto& obstacle : obstacles) {
        rect = {obstacle.first * dl, menu_bar_height + obstacle.second * dl, dl, dl};
        SDL_RenderFillRect(renderer, &rect);
    }

    // Draw Current State Of A*
    short state = aStar.getState();
    if (state == SIMULATING || state == FINISHED) {
        auto openSet   = aStar.getOpenSet();
        auto closedSet = aStar.getClosedSet();

        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        for (const auto& tile : openSet) {
            rect = {tile.second.first * dl, menu_bar_height + tile.second.second * dl, dl, dl};
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for (const auto& tile : closedSet) {
            rect = {tile.first * dl, menu_bar_height + tile.second * dl, dl, dl};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    // Draw A* Result
    if (state == FINISHED) {
        auto final_path = aStar.getFinalPath();
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        for (const auto& tile : final_path) {
            rect = {tile.first * dl, menu_bar_height + tile.second * dl, dl, dl};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    // Draw Start/Target
    auto start  = aStar.getStart();
    auto target = aStar.getTarget();

    // Start
    color = aStar.getStartColor();
    SDL_SetRenderDrawColor(renderer, 
            (Uint8)(color.x * 255), 
            (Uint8)(color.y * 255), 
            (Uint8)(color.z * 255), 
            (Uint8)(color.w * 255));

    rect = {start.first * dl, menu_bar_height + start.second * dl, dl, dl};
    SDL_RenderFillRect(renderer, &rect);

    // Target
    color = aStar.getTargetColor();
    SDL_SetRenderDrawColor(renderer, 
            (Uint8)(color.x * 255), 
            (Uint8)(color.y * 255), 
            (Uint8)(color.z * 255), 
            (Uint8)(color.w * 255));

    rect = {target.first * dl, menu_bar_height + target.second * dl, dl, dl};
    SDL_RenderFillRect(renderer, &rect);

    // Draw Grid
    if (aStar.gridIsShown()) {
        color = aStar.getGridColor();
        SDL_SetRenderDrawColor(renderer, 
                (Uint8)(color.x * 255), 
                (Uint8)(color.y * 255), 
                (Uint8)(color.z * 255), 
                (Uint8)(color.w * 255));

        auto dims = aStar.getDimensions();
        for (i32 i = 1; i < dims.first; i++) {
            SDL_RenderDrawLine(renderer, 
                    i * dl, 
                    menu_bar_height, 
                    i * dl, 
                    menu_bar_height + HEIGHT);
        }

        for (i32 i = 1; i < dims.second; i++) {
            SDL_RenderDrawLine(renderer, 
                    0, 
                    menu_bar_height + i * dl, 
                    WIDTH, 
                    menu_bar_height + i * dl);
        }
    }
}

void Visualization::run() 
{
    while (running) {
        Input();
        Update();
        Render();
    }
}
