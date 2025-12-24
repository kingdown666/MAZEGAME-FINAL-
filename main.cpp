#include <raylib.h>
#include <format>
#include "maze.h"
#include "player.h"
#include <iostream>
#include "path_finder.h" 
#include "path_renderer.h" 
using namespace std;
const string maze_file = "resources/maze1.txt";

bool showDFS = false;    // DFS路径显示状态
bool showBFS = false;    // BFS路径显示状态
bool showDijkstra = false; // Dijkstra路径显示状态


int main() {
    InitWindow(1400, 1000, "Game");
    if (!IsWindowReady()) {
        std::cout << "Failed to create window!" << std::endl;
        return 1;
    }
    Maze maze(maze_file);
    player player(maze);
    PathFinder path_finder(maze);
    vector<Coordinate> dfs_path = path_finder.dfs();
    vector<Coordinate> bfs_path = path_finder.bfs();
    vector<Coordinate> dijkstra_path = path_finder.dijkstra();

    Coordinate end_coord = maze.get_end_Coord();
    bool game_won = false;

    Rectangle dfsButton = { 1200, 20, 120, 40 };   // 右侧DFS按钮
    Rectangle bfsButton = { 1200, 80, 120, 40 };   // 右侧BFS按钮
	Rectangle dijkstraButton = { 1200, 140, 120, 40 }; // 右侧Dijkstra按钮


    while (!WindowShouldClose()) {
// 键盘切换路径显示
        if (IsKeyPressed(KEY_ONE)) {  // 按1键切换DFS
            showDFS = !showDFS;
            showBFS = false;
            showDijkstra = false;
        }
        else if (IsKeyPressed(KEY_TWO)) {  // 按2键切换BFS
            showBFS = !showBFS;
            showDFS = false;
            showDijkstra = false;
        }
        else if (IsKeyPressed(KEY_THREE)) {  // 按3键切换Dijkstra
            showDijkstra = !showDijkstra;
            showDFS = false;
            showBFS = false;
        }
        else if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_BACKSPACE)) {  // 按0或退格键隐藏所有路径
            showDFS = false;
            showBFS = false;
            showDijkstra = false;
        }
        // 处理按钮点击事件
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            // 检查DFS按钮点击
            if (CheckCollisionPointRec(mousePos, dfsButton)) {
                showDFS = !showDFS;  // 切换显示状态
                showBFS = false;     // 隐藏另一种路径
				showDijkstra = false; // 隐藏Dijkstra路径
            }
            // 检查BFS按钮点击
            else if (CheckCollisionPointRec(mousePos, bfsButton)) {
                showBFS = !showBFS;  // 切换显示状态
                showDFS = false;     // 隐藏另一种路径
				showDijkstra = false; // 隐藏Dijkstra路径
            }
            else if (CheckCollisionPointRec(mousePos, dijkstraButton)) {
                showDijkstra = !showDijkstra;  // 切换显示状态
                showDFS = false;               // 隐藏其他路径
                showBFS = false;               // 隐藏其他路径
            }
        }

        if (!game_won) {
            player.update();
            if (player.get_curr_coord().x == end_coord.x &&
                player.get_curr_coord().y == end_coord.y) {
                game_won = true;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        maze.draw();
        player.draw();

      
        if (showDFS) {
            PathRenderer::draw_path(dfs_path, maze, RED);
        }
        if (showBFS) {
            PathRenderer::draw_path(bfs_path, maze, BLUE);
        }
        if (showDijkstra) {
            PathRenderer::draw_path(dijkstra_path, maze, PURPLE);
        }

       
        // 在绘制按钮的部分，修改DrawText的参数：

        DrawRectangleRec(dfsButton, showDFS ? GREEN : LIGHTGRAY);
        DrawText("Show DFS (1)", dfsButton.x + 10, dfsButton.y + 10, 16, BLACK);  // 添加(1)

        DrawRectangleRec(bfsButton, showBFS ? GREEN : LIGHTGRAY);
        DrawText("Show BFS (2)", bfsButton.x + 10, bfsButton.y + 10, 16, BLACK);  // 添加(2)

        DrawRectangleRec(dijkstraButton, showDijkstra ? GREEN : LIGHTGRAY);
        DrawText("Show Dijkstra (3)", dijkstraButton.x + 5, dijkstraButton.y + 10, 14, BLACK);  // 修改文本
        // 在绘制游戏信息的区域添加键盘控制提示（可以在绘制按钮后添加）：

// 绘制键盘控制提示
        DrawText("Keyboard Controls:", 1200, 200, 18, BLACK);
        DrawText("1 - Toggle DFS Path", 1200, 230, 16, DARKGRAY);
        DrawText("2 - Toggle BFS Path", 1200, 260, 16, DARKGRAY);
        DrawText("3 - Toggle Dijkstra", 1200, 290, 16, DARKGRAY);
        DrawText("0 - Hide All Paths", 1200, 320, 16, DARKGRAY);


        if (game_won) {
            DrawRectangle(0, 180, 800, 120, Fade(BLACK, 0.7f));
            DrawText("WIN THE GAME!", 220, 220, 40, YELLOW);
            DrawText("Press any key to restart", 250, 270, 20, WHITE);
        }

     
        DrawText(std::format("FPS: {}", GetFPS()).c_str(), 5, 5, 20, WHITE);
        DrawText("Use arrow keys to move", 1000, 240, 20, BLACK);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}