#ifndef PATH_FINDER_H
#include "maze.h"
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include <set>

class PathFinder {
private:
   const Maze& maze;
   Coordinate start;
   Coordinate end;

   // 检查坐标是否有效
   bool is_valid(Coordinate c, const std::vector<std::vector<bool>>& visited);
   int get_cost(Coordinate c);


public:
   PathFinder(const Maze& m) : maze(m) {
       start = m.get_start_Coord();
       end = m.get_end_Coord(); // 从迷宫获取终点
   }

   // 显式声明复制构造函数和赋值运算符
   PathFinder(const PathFinder& other) = default;
   PathFinder& operator=(const PathFinder& other) = default;

   // 深度优先搜索（DFS）
   std::vector<Coordinate> dfs();

   // 广度优先搜索（BFS）
   std::vector<Coordinate> bfs();
   // Dijkstra算法求最短路径
   std::vector<Coordinate> dijkstra();
   // Dijkstra算法，允许经过一次熔岩地板
   std::vector<Coordinate> dijkstra_with_one_lava();
};

#endif // PATH_FINDER_H