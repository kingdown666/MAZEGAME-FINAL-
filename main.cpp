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
bool gameStarted = false;  // 游戏是否开始
bool game_won = false;     // 游戏是否胜利
bool game_failed = false;  // 游戏是否失败（新添加）
bool showDijkstraOneLava = false; // 允许踩一次熔岩的Dijkstra路径显示状态

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
    vector<Coordinate> dijkstra_one_lava_path = path_finder.dijkstra_with_one_lava();

    Coordinate end_coord = maze.get_end_Coord();
    bool game_won = false;

    Rectangle dfsButton = { 1200, 20, 120, 40 };   // 右侧DFS按钮
    Rectangle bfsButton = { 1200, 80, 120, 40 };   // 右侧BFS按钮
	Rectangle dijkstraButton = { 1200, 140, 120, 40 }; // 右侧Dijkstra按钮
    Rectangle dijkstraOneLavaButton = { 1200, 200, 140, 40 }; // 允许踩一次熔岩的Dijkstra按钮


    while (!WindowShouldClose()) {
        // 游戏开始界面检测
        if (!gameStarted) {
            if (IsKeyPressed(KEY_SPACE)) {
                gameStarted = true;
                game_won = false;
                game_failed = false;
                player.reset_lava_count(); // 重置熔岩计数
            }

            // 只渲染开始界面，不执行游戏逻辑
            BeginDrawing();
            ClearBackground(BLACK);  // 使用黑色背景让开始界面更突出

            // 绘制游戏标题和说明
            DrawText("Maze Path Finding Game", 400, 100, 70, YELLOW);
            DrawRectangle(350, 200, 700, 400, DARKGRAY);
            DrawText("Goal: Go from Start(S) to End(E)", 380, 220, 25, WHITE);
            DrawText("Controls:", 380, 270, 25, WHITE);
            DrawText("Arrow Keys - Move Character", 400, 310, 22, LIGHTGRAY);
            DrawText("Keys 1, 2, 3 - Show different algorithm paths", 400, 340, 22, LIGHTGRAY);
            DrawText("Key 0 - Hide all paths", 400, 370, 22, LIGHTGRAY);
            DrawText("ESC - Exit Game", 400, 400, 22, LIGHTGRAY);
            DrawText("Key 4 - Show one-lava Dijkstra path", 400, 370, 22, LIGHTGRAY);

            // 绘制开始游戏提示
            DrawRectangle(400, 500, 400, 80, DARKBLUE);
            DrawText("Press SPACE to Start Game", 450, 525, 40, GREEN);

            // 闪烁效果
            static float blinkTimer = 0;
            blinkTimer += GetFrameTime();
            if (fmod(blinkTimer, 1.0f) < 0.5f) {
                DrawText(">", 420, 535, 40, GREEN);
                DrawText("<", 760, 535, 40, GREEN);
            }
            EndDrawing();
            continue;  // 跳过游戏逻辑，直接进入下一次循环
        }
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
        else if (IsKeyPressed(KEY_FOUR)) {  // 按4键切换允许踩一次熔岩的Dijkstra
            showDijkstraOneLava = !showDijkstraOneLava;
            showDFS = false;
            showBFS = false;
            showDijkstra = false;
        }
        else if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_BACKSPACE)) {  // 按0或退格键隐藏所有路径
            showDFS = false;
            showBFS = false;
            showDijkstra = false;
            showDijkstraOneLava = false; // 新添加
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
            // 检测允许踩一次熔岩的Dijkstra按钮点击
            else if (CheckCollisionPointRec(mousePos, dijkstraOneLavaButton)) {
                showDijkstraOneLava = !showDijkstraOneLava;
                showDFS = false;
                showBFS = false;
                showDijkstra = false;
            }
        }

        // 游戏失败检测：当小人第二次踩到熔岩地板时
        if (!game_won && !game_failed) {
            player.update();
            // 检测熔岩步数
            if (player.get_lava_step_count() >= 2) {
                game_failed = true;
            }
            // 检测是否到达终点
            if (player.get_curr_coord().x == end_coord.x &&
                player.get_curr_coord().y == end_coord.y) {
                game_won = true;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        maze.draw();
        player.draw();

		// 根据显示状态绘制路径
        if (showDFS) {
            PathRenderer::draw_path(dfs_path, maze, RED);
        }
        if (showBFS) {
            PathRenderer::draw_path(bfs_path, maze, BLUE);
        }
        if (showDijkstra) {
            PathRenderer::draw_path(dijkstra_path, maze, PURPLE);
        }
        if (showDijkstraOneLava) {  // 新添加
            PathRenderer::draw_path(dijkstra_one_lava_path, maze, ORANGE);
        }

       
        // 在绘制按钮的部分，修改DrawText的参数：

        DrawRectangleRec(dfsButton, showDFS ? GREEN : LIGHTGRAY);
        DrawText("Show DFS (1)", dfsButton.x + 10, dfsButton.y + 10, 16, BLACK);  // 添加(1)

        DrawRectangleRec(bfsButton, showBFS ? GREEN : LIGHTGRAY);
        DrawText("Show BFS (2)", bfsButton.x + 10, bfsButton.y + 10, 16, BLACK);  // 添加(2)

        DrawRectangleRec(dijkstraButton, showDijkstra ? GREEN : LIGHTGRAY);
        DrawText("Show Dijkstra (3)", dijkstraButton.x + 5, dijkstraButton.y + 10, 14, BLACK);  // 修改文本
        // 在绘制游戏信息的区域添加键盘控制提示（可以在绘制按钮后添加）：
        // 绘制当前状态信息
        DrawText(std::format("Lava Steps: {}/2", player.get_lava_step_count()).c_str(),
            1200, 360, 20, player.get_lava_step_count() >= 1 ? ORANGE : BLACK);
		// 绘制允许踩一次熔岩的Dijkstra按钮
        DrawRectangleRec(dijkstraOneLavaButton, showDijkstraOneLava ? GREEN : LIGHTGRAY);
        DrawText("One-Lava Dijk (4)", (int)dijkstraOneLavaButton.x + 5, (int)dijkstraOneLavaButton.y + 10, 14, BLACK);

        // 如果站在草地上，显示减速提示
        TileType current_tile = maze.get_tile_type(player.get_curr_coord());
        if (current_tile == GRASS) {
            DrawText("On Grass: Speed Reduced!", 1000, 400, 20, DARKGREEN);
        }
// 绘制键盘控制提示
        DrawText("Keyboard Controls:", 1200, 200, 18, BLACK);
        DrawText("1 - Toggle DFS Path", 1200, 230, 16, DARKGRAY);
        DrawText("2 - Toggle BFS Path", 1200, 260, 16, DARKGRAY);
        DrawText("3 - Toggle Dijkstra", 1200, 290, 16, DARKGRAY);
        DrawText("4 - One-Lava Dijkstra", 1200, 370, 16, DARKGRAY);
        DrawText("0 - Hide All Paths", 1200, 320, 16, DARKGRAY);


        if (game_won) {
            DrawRectangle(0, 180, 800, 120, Fade(BLACK, 0.7f));
            DrawText("WIN THE GAME!", 220, 220, 40, YELLOW);
            DrawText("Press any key to restart", 250, 270, 20, WHITE);
            // 添加重新开始游戏的检测
            if (IsKeyPressed(KEY_SPACE)) {
                // 重置游戏状态
                game_won = false;
                game_failed = false;  // 失败设定
                // 将玩家重置到起点
                player.reset();  // 使用新添加的reset函数
                // 可以重新计算路径（可选）
                dfs_path = path_finder.dfs();
                bfs_path = path_finder.bfs();
                dijkstra_path = path_finder.dijkstra();
                // 隐藏所有路径
                showDFS = false;
                showBFS = false;
                showDijkstra = false;
                showDijkstraOneLava = false; // 新添加
            }
        }
        // 游戏失败界面
        if (game_failed) {
            DrawRectangle(0, 180, 800, 120, Fade(BLACK, 0.7f));
            DrawText("GAME OVER!", 250, 220, 50, RED);
            DrawText("Stepped on lava twice!", 220, 270, 30, ORANGE);
            DrawText("Press SPACE to restart", 250, 300, 20, WHITE);

            // 添加重新开始游戏的检测
            if (IsKeyPressed(KEY_SPACE)) {
                // 重置游戏状态
                game_won = false;
                game_failed = false;
                // 将玩家重置到起点
                player.reset();  // 使用新添加的reset函数
                // 可以重新计算路径（可选）
                dfs_path = path_finder.dfs();
                bfs_path = path_finder.bfs();
                dijkstra_path = path_finder.dijkstra();
                // 隐藏所有路径
                showDFS = false;
                showBFS = false;
                showDijkstra = false;
                showDijkstraOneLava = false; // 新添加
            }
        }
     
        DrawText(std::format("FPS: {}", GetFPS()).c_str(), 5, 5, 20, WHITE);
        DrawText("Use arrow keys to move", 1000, 240, 20, BLACK);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}