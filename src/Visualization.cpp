#include "Visualization.hpp"

Visualization::Visualization()
{
    aStar = AStar({0, 0}, {BASE_WIDTH - 1, BASE_HEIGHT - 1});
    aStar.setDimensions({BASE_WIDTH, BASE_HEIGHT});
    aStar.calcDeltaLength(WIDTH, HEIGHT);
    aStar.setSelected(NONE);
    aStar.setDelay(50);

    float color = 70.0f / 255.0f;
    background_color = {color, color, color, 1.0f};
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

            case SDL_KEYDOWN:
                OnKeyDown(e);
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
        AboutMenu();
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

        ImGui::BeginDisabled(!aStar.stateEditing() || edit_stack.getUndoSize() == 0);
        if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
            OnUndo(); 
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled(!aStar.stateEditing() || edit_stack.getRedoSize() == 0);
        if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
            OnRedo();
        }
        ImGui::EndDisabled();

        ImGui::EndMenu();
    }
}

void Visualization::RunMenu() 
{
    if (ImGui::BeginMenu("Run")) {
        menu_open = true;
        
        ImGui::BeginDisabled(!aStar.stateEditing());
        if (ImGui::MenuItem("Run", "Ctrl+R")) {
            aStar.startSimulation();
        }
        ImGui::EndDisabled();

        std::string label;
        if (aStar.getState() == SIMULATING) {
            label = "Abort";
        } else {
            label = "Finish";
        }

        ImGui::BeginDisabled(aStar.stateEditing());
        if (ImGui::MenuItem(label.c_str(), "Ctrl+A")) {
            if (aStar.getState() == SIMULATING) {
                aStar.setState(FINISHED);
            } else {
                aStar.setState(EDITING);
            }
        }
        ImGui::EndDisabled();

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

        ImGui::BeginDisabled(!aStar.stateEditing());
        if (ImGui::MenuItem("Clear Obstacles", "Ctrl+D")) {
            aStar.clearObstacles();
        }
        ImGui::EndDisabled();

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

void Visualization::AboutMenu()
{
    if (ImGui::BeginMenu("About")) {
        ImGui::Text("Made by:\n Marcos Leonardo Ramirez Joos");

        ImGui::Separator(); 

        if (ImGui::BeginMenu("Instructions")) { 
            ImGui::EndMenu();
        }

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

        stack_pair.first = {mouse_pos};
        if (mouse_pos == aStar.getTarget()) {
            aStar.setSelected(TARGET);
            stack_pair.second = MOVE_TARGET;
        } else if (mouse_pos == aStar.getStart()) {
            aStar.setSelected(START);
            stack_pair.second = MOVE_START;
        } else if (obstacles.find(mouse_pos) != obstacles.end()) {
            aStar.setSelected(OBSTACLE);
            aStar.removeObstacle(mouse_pos);
            stack_pair.second = DELETE_OBST;
        } else {
            aStar.setSelected(BLANCK);
            aStar.addObstacle(mouse_pos);
            stack_pair.second = INSERT_OBST;
        }
    }
}

void Visualization::OnMouseButtonUp(const SDL_Event& e)
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    auto mouse_pos = aStar.mouseGetOver(x, y - menu_bar_height);

    if (!menu_open && aStar.getSelected() != NONE && e.button.button == SDL_BUTTON_LEFT) {
        aStar.setSelected(NONE);

        if (stack_pair.second == MOVE_START || stack_pair.second == MOVE_TARGET) {
            stack_pair.first.push_back(mouse_pos);
        }

        edit_stack.WriteStack(stack_pair.first, stack_pair.second);
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
            stack_pair.first.push_back(mouse_pos);
        } else if (aStar.getSelected() == BLANCK && !mouse_on_other_tile) {
            bool added = false;
            for (const auto& p : stack_pair.first) {
                if (p == mouse_pos) {
                    added = true;
                }
            }

            auto obsts = aStar.getObstacles();
            if (obsts.find(mouse_pos) == obsts.end() && !added) {
                aStar.addObstacle(mouse_pos);
                stack_pair.first.push_back(mouse_pos);
            }
        }
    }
}

void Visualization::OnKeyDown(const SDL_Event& e)
{
    auto mod = e.key.keysym.mod;
    auto key = e.key.keysym.sym;

    if (mod & KMOD_LCTRL || mod & KMOD_RCTRL) {
        switch (key) {
            case SDLK_r:
                aStar.startSimulation();
                break;

            case SDLK_a:
                if (aStar.getState() == SIMULATING) {
                    aStar.setState(FINISHED);
                } else {
                    aStar.setState(EDITING);
                }
                break;

            case SDLK_d:
                aStar.clearObstacles();
                break;

            case SDLK_z:
                if (edit_stack.getUndoSize() > 0) OnUndo();
                break;

            case SDLK_y:
                if (edit_stack.getRedoSize() > 0) OnRedo();
                break;
        }
    }
}

void Visualization::OnUndo()
{
    short op;
    auto data = edit_stack.Undo(&op);

    switch (op) {
        case MOVE_START:
            aStar.setStart(data[0]);
            break;

        case MOVE_TARGET:
            aStar.setTarget(data[0]);
            break;

        case INSERT_OBST:
            for (const auto& p : data) {
                aStar.removeObstacle(p);
            }
            break;

        case DELETE_OBST:
            aStar.addObstacle(data);
            break;
    }
}

void Visualization::OnRedo()
{
    short op;
    auto data = edit_stack.Redo(&op);

    switch (op) {
        case MOVE_START:
            aStar.setStart(data[1]);
            break;

        case MOVE_TARGET:
            aStar.setTarget(data[1]);
            break;

        case INSERT_OBST:
            aStar.addObstacle(data);
            break;

        case DELETE_OBST:
            for (const auto& p : data) {
                aStar.removeObstacle(p);
            }
            break;
            
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
