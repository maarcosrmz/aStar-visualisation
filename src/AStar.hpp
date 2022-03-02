#ifndef GRID_HPP
#define GRID_HPP

#include <SDL2/SDL.h>

#include "imgui/imgui.h"

#include "Util.hpp"

#define BASE_WIDTH 16
#define BASE_HEIGHT 9

enum States {
    EDITING,
    SIMULATING,
    FINISHED
};

enum Tiles {
    NONE,
    BLANCK,
    OBSTACLE, 
    TARGET,
    START
};

class AStar {

    private:
        // Attributes
        bool show_grid;
        
        short state;
        short selected;

        i32 delta_length;
        i32 scalar;
        i32 delay;

        std::pair<i32, i32> dimensions;

        std::pair<i32, i32> start;
        std::pair<i32, i32> target;

        std::unordered_set<std::pair<i32, i32>, pair_hash> obstacle_tiles;

        std::set<std::pair<i32, std::pair<i32, i32>>> openSet;
        std::unordered_set<std::pair<i32, i32>, pair_hash> closedSet;

        std::vector<std::pair<i32, i32>> final_path;

        ImVec4 start_color;
        ImVec4 target_color;
        ImVec4 obstacle_color;
        ImVec4 grid_color;

        std::thread* sim_thread;

        // A* Algorithm
        void aStarPathfinding();
        void retracePath(
                const std::pair<i32, i32> &current, 
                const std::unordered_map<std::pair<i32, i32>, 
                    std::pair<i32, i32>, pair_hash> &parents);
        std::vector<std::pair<i32, i32>> getAdjacentSquares(
                const std::pair<i32, i32> &current);
        i32 heuristic(
                const std::pair<i32, i32> &a, 
                const std::pair<i32, i32> &b);

    public:
        AStar();
        AStar(const std::pair<i32, i32>& start, const std::pair<i32, i32>& target);
        ~AStar();

        // A* Algorithm
        void startSimulation();

        // Dimensions
        void calcDeltaLength(i32 win_w, i32 win_h);

        // Obstacles
        void addObstacle(const std::pair<i32, i32>& new_obst);
        void removeObstacle(const std::pair<i32, i32>& obst);
        void clearObstacles();

        // Mouse
        std::pair<i32, i32> mouseGetOver(i32 x_mouse, i32 y_mouse);
        bool mouseOutOfBounds(std::pair<i32, i32> mouse_pos);
        bool mouseOnOtherTile(std::pair<i32, i32> mouse_pos, short selected);

        // Setters
        void showGrid();
        void hideGrid();

        void setState(short state);
        void setSelected(short selected);
        void setScalar(i32 scalar);
        void setDelay(i32 delay);
        void setDimensions(std::pair<i32, i32> dimensions);
        void setStart(const std::pair<i32, i32>& start);
        void setTarget(const std::pair<i32, i32>& target);
        void setObstacles(const std::vector<std::pair<i32, i32>>& obstacle_tiles);
        void setStartColor(ImVec4 start_color);
        void setTargetColor(ImVec4 target_color);
        void setObstacleColor(ImVec4 obstacle_color);
        void setGridColor(ImVec4 grid_color);

        // Getters
        inline short
            getState() const { return state; }
        inline short
            getSelected() const { return selected; }
        inline i32 
            getDeltaLength() const { return delta_length; }
        inline i32 
            getScalar() const { return scalar; }
        inline i32 
            getDelay() const { return delay; }
        inline bool 
            gridIsShown() const { return show_grid; }
        inline std::pair<i32, i32> 
            getDimensions() const { return dimensions; }
        inline std::pair<i32, i32> 
            getStart() const { return start; }
        inline std::pair<i32, i32> 
            getTarget() const { return target; }
        inline std::unordered_set<std::pair<i32, i32>, pair_hash> 
            getObstacles() const { return obstacle_tiles; }
        inline std::vector<std::pair<i32, i32>> 
            getFinalPath() const { return final_path; }
        inline ImVec4 
            getStartColor() const { return start_color; }
        inline ImVec4 
            getTargetColor() const { return target_color; }
        inline ImVec4 
            getObstacleColor() const { return obstacle_color; }
        inline ImVec4 
            getGridColor() const { return grid_color; }
        inline std::set<std::pair<i32, std::pair<i32, i32>>> 
            getOpenSet() const { return openSet; };
        inline std::unordered_set<std::pair<i32, i32>, pair_hash> 
            getClosedSet() const { return closedSet; };
};

#endif //GRID_HPP
