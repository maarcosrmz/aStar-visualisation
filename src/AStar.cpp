#include "AStar.hpp"

AStar::AStar()
{
    float sec  =  50.0f / 255.0f;
    float prim = 175.0f / 255.0f;

    start_color    = {prim,  sec,  sec, 1.0f};
    obstacle_color = { sec, prim,  sec, 1.0f};
    target_color   = { sec,  sec, prim, 1.0f};
    grid_color     = {0.0f, 0.0f, 0.0f, 1.0f};
    open_color     = {prim, prim,  sec, 1.0f};
    closed_color   = { sec, prim, prim, 1.0f};

    state = EDITING;
    scalar = 1;
    sim_thread = nullptr;
}

AStar::AStar(
        const std::pair<i32, i32>& start, 
        const std::pair<i32, i32>& target)
{
    this->start  = start;
    this->target = target;

    float sec  =  50.0f / 255.0f;
    float prim = 175.0f / 255.0f;

    start_color    = {prim,  sec,  sec, 1.0f};
    obstacle_color = { sec, prim,  sec, 1.0f};
    target_color   = { sec,  sec, prim, 1.0f};
    grid_color     = {0.0f, 0.0f, 0.0f, 1.0f};
    open_color     = {prim, prim,  sec, 1.0f};
    closed_color   = { sec, prim, prim, 1.0f};

    state = EDITING;
    scalar = 1;
    sim_thread = nullptr;
}

AStar::~AStar()
{
    delete sim_thread;
    sim_thread = nullptr;
}

void AStar::aStarPathfinding()
{
    std::unordered_map<std::pair<i32, i32>, i32, pair_hash> gScore;

    std::unordered_map<std::pair<i32, i32>, std::pair<i32, i32>, pair_hash> parents;

    fScore[start] = gScore[start] = heuristic(start, target); 

    openSet.insert(std::pair<i32, std::pair<i32, i32>>(fScore[start], start));

    while (!openSet.empty() && state == SIMULATING) {
        std::pair<i32, i32> current = openSet.begin()->second;

        if (current == target) {
            retracePath(current, parents);
            state = FINISHED;
        }

        openSet.erase(openSet.begin());
        closedSet.insert(current);

        std::vector<std::pair<i32, i32>> adjacentSquares = getAdjacentSquares(current);
        for (const std::pair<i32, i32> &square : adjacentSquares) {
            if (obstacle_tiles.find(square) != obstacle_tiles.end() || closedSet.find(square) != closedSet.end()) {
                continue;
            } else if (fScore.find(square) == fScore.end()) {
                gScore[square] = gScore[current] + heuristic(current, square);
                fScore[square] = gScore[square]  + heuristic(square, target) * (1.0f + TIE_BREAKER);
                parents[square] = current;

                openSet.insert(std::pair<i32, std::pair<i32, i32>>(fScore[square], square));
            } else {
                i32 new_gScore = gScore[current] + heuristic(current, square);
                if (new_gScore < gScore[square]) {
                    openSet.erase(std::pair<i32, std::pair<i32, i32>>(fScore[square], square)); 

                    gScore[square] = new_gScore;
                    fScore[square] = gScore[square] + heuristic(square, target) * (1.0f * TIE_BREAKER);
                    parents[square] = current;

                    openSet.insert(std::pair<i32, std::pair<i32, i32>>(fScore[square], square)); 
                }
            }

            SDL_Delay(delay);
        }
    }

    state = FINISHED;
}

void AStar::retracePath(
        const std::pair<i32, i32> &current, 
        const std::unordered_map<std::pair<i32, i32>, 
            std::pair<i32, i32>, pair_hash> &parents) 
{
    final_path = {current};

    auto parent = current;
    do {
        parent = parents.find(parent)->second; 
        final_path.push_back(parent);
    } while (parents.find(parent) != parents.end());
}

std::vector<std::pair<i32, i32>> AStar::getAdjacentSquares(
        const std::pair<i32, i32> &current)
{
    std::vector<std::pair<i32, i32>> adjacentSquares;

    i32 f = current.first;
    i32 s = current.second;

    if (f > 0) adjacentSquares.push_back(std::pair<i32, i32>(f - 1, s));
    if (s > 0) adjacentSquares.push_back(std::pair<i32, i32>(f, s - 1));
    if (f < dimensions.first - 1) adjacentSquares.push_back(std::pair<i32, i32>(f + 1, s));
    if (s < dimensions.second - 1) adjacentSquares.push_back(std::pair<i32, i32>(f, s + 1));

    return adjacentSquares; 
}

i32 AStar::heuristic(const std::pair<i32, i32> &a, const std::pair<i32, i32> &b) const {
    i32 dx = abs(a.first  - b.first);
    i32 dy = abs(a.second - b.second);
    
    return 10 * (dx + dy);
}

void AStar::addObstacle(const std::pair<i32, i32>& new_obst)
{
    obstacle_tiles.insert(new_obst);
}

void AStar::addObstacle(const std::vector<std::pair<i32, i32>>& obst)
{
    obstacle_tiles.insert(obst.begin(), obst.end());
}

void AStar::removeObstacle(const std::pair<i32, i32>& obst)
{
    obstacle_tiles.erase(obst);
}

void AStar::clearObstacles()
{
    obstacle_tiles.clear();
}

void AStar::startSimulation()
{
    state = SIMULATING;

    openSet.clear();
    closedSet.clear();
    fScore.clear();
    final_path.clear();

    sim_thread = new std::thread( [this] { aStarPathfinding(); } );
    sim_thread->detach();

    delete sim_thread;
    sim_thread = nullptr;
}

bool AStar::stateEditing() const 
{
    if (state == EDITING) {
        return true;
    } else {
        return false;
    }
}

void AStar::calcDeltaLength(i32 win_w, i32 win_h)
{
    i32 dx = win_w / dimensions.first;
    i32 dy = win_h / dimensions.second;

    if (dx != dy) {
        throw std::runtime_error("The aspect ratio of the window and the grid do not coincide!");
    }

    delta_length = dx;
}

void AStar::showGrid()
{
    show_grid = true;
}

void AStar::hideGrid()
{
    show_grid = false;
}

std::pair<i32, i32> AStar::mouseGetOver(i32 x_mouse, i32 y_mouse) const
{
    std::pair<i32, i32> mouse_grid_pos = {-1, -1};

    if (x_mouse < 0 || y_mouse < 0) {
        return mouse_grid_pos; 
    }

    mouse_grid_pos = {
        x_mouse / delta_length, 
        y_mouse / delta_length 
    };

    return mouse_grid_pos;
}

bool AStar::mouseOutOfBounds(std::pair<i32, i32> mouse_pos) const
{
    bool not_on_grid_x = mouse_pos.first  < 0 || mouse_pos.first  >= dimensions.first;
    bool not_on_grid_y = mouse_pos.second < 0 || mouse_pos.second >= dimensions.second;

    if (not_on_grid_x || not_on_grid_y) {
        return true;
    }

    return false;
}

bool AStar::mouseOnOtherTile(std::pair<i32, i32> mouse_pos, short selected) const
{
    bool mouse_obst = obstacle_tiles.find(mouse_pos) != obstacle_tiles.end();

    switch (selected) {
        case START:
            if (mouse_pos == target || mouse_obst) {
                return true;
            }
            break;

        case TARGET:
            if (mouse_pos == start || mouse_obst) {
                return true;
            }
            break;

        case OBSTACLE:
        case BLANCK:
            if (mouse_pos == start || mouse_pos == target) {
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

void AStar::setState(short state)
{
    this->state = state; 
}

void AStar::setSelected(short selected)
{
    this->selected = selected; 
}

void AStar::setScalar(i32 scalar) 
{
    this->scalar = scalar;

    dimensions = {BASE_WIDTH * scalar, BASE_HEIGHT * scalar};

    if (start.first >= dimensions.first || start.second >= dimensions.second) {
        start.first  = 0;
        start.second = 0;
    }

    if (target.first >= dimensions.first || target.second >= dimensions.second) {
        target.first  = dimensions.first  - 1;
        target.second = dimensions.second - 1;
    }
}

void AStar::setDelay(i32 delay)
{
    this->delay = delay;
}

void AStar::setDimensions(std::pair<i32, i32> dimensions)
{
    this->dimensions = dimensions;

    i32 dx = dimensions.first  / BASE_WIDTH;
    i32 dy = dimensions.second / BASE_HEIGHT;

    if (dx != dy) {
        throw std::runtime_error("The aspect ratio of the window and the grid do not coincide!");
    }

    scalar = dx;
}

void AStar::setStart(const std::pair<i32, i32>& start)
{
    this->start = start;
}

void AStar::setTarget(const std::pair<i32, i32>& target)
{
    this->target = target;
}

void AStar::setObstacles(const std::vector<std::pair<i32, i32>>& obstacle_tiles)
{
    this->obstacle_tiles = std::unordered_set<std::pair<i32, i32>, pair_hash>(obstacle_tiles.begin(), obstacle_tiles.end());
}

void AStar::setStartColor(ImVec4 start_color)
{
    this->start_color = start_color;
}

void AStar::setTargetColor(ImVec4 target_color)
{
    this->target_color = target_color;
}

void AStar::setObstacleColor(ImVec4 obstacle_color)
{
    this->obstacle_color = obstacle_color;
}

void AStar::setGridColor(ImVec4 grid_color)
{
    this->grid_color = grid_color;
}

void AStar::setOpenColor(ImVec4 open_color)
{
    this->open_color = open_color;
}

void AStar::setClosedColor(ImVec4 closed_color)
{
    this->closed_color = closed_color;
}

void AStar::setClosedColorStatic(bool isStatic)
{
    this->static_closedColor = isStatic;
}
