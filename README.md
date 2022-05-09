# aStar-pathfinding-visualization
Simple implementation of the A* path-finding algorithm, which is additionally visualised.

## Description
The application uses [Dear ImGui](https://github.com/ocornut/imgui) as an user interface, and [SDL2](https://github.com/libsdl-org/SDL) for rendering to the screen and getting user input.

This application visualises an implementation of the A* algorithm. The user can adapt the grid by pressing on a cell to place an obstacle, and pressing on an obstacle to remove it. The start (red) and target (blue) cells can be moved around by dragging them to the desired location. Both the grid size can be changed and the execution/visualization speed adjusted by editing the properties of each in the according menus.

The menu bar has different sections for different purposes. The _Edit_ menu is for undoing or redoing certain editing actions. In the _Run_ menu you can run and stop the algorithm visualization, as well as set the speed (or delay) of the visualization. The _Grid_ menu is used to set the grid size and choose, whether or not the grid should be shown. And last but not least, in the _Color_ menu you can change the colors for different aspects of the visualization (i.e. background, grid, etc.).

## Screenshots
![Screenshot of raw application screen](https://raw.githubusercontent.com/maarcosrmz/aStar-visualisation/main/screenshots/AStar1.png)

![Screenshot of grid with some obstacles](https://raw.githubusercontent.com/maarcosrmz/aStar-visualisation/main/screenshots/AStar2.png)

![Screenshot of grid with drawn name of the app](https://raw.githubusercontent.com/maarcosrmz/aStar-visualisation/main/screenshots/AStar3.png)

## Download
```
git clone https://github.com/maarcosrmz/aStar-visualisation
```

## Compile and run
```
cd aStar-visualisation && make
```

## License
This software is licensed under the MIT License, see [LICENSE.txt](https://github.com/maarcosrmz/aStar-visualisation/blob/main/LICENSE.txt) for more information.
