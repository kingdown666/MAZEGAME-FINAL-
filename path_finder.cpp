#include "path_finder.h"
// 关键修正：适配 maze0.txt 的可通行规则（排除 WALL=1，允许 FLOOR/GRASS/LAVA）
bool PathFinder::is_valid(Coordinate c, const std::vector<std::vector<bool>>& visited) {
    // 1. 检查坐标是否越界（适配 10x10 迷宫）
    if (c.x < 0 || c.y < 0) return false;
    if (c.y >= maze.get_rows() || c.x >= maze.get_cols()) return false;

    // 2. 检查是否为墙壁（WALL=1，不可通行）
    if (maze.get_tile_type(c) == WALL) return false;

    // 3. 检查是否已访问
    if (visited[c.y][c.x]) return false;

    return true; // 0(FLOOR)/2(GRASS)/3(LAVA) 均可通行
}
int PathFinder::get_cost(Coordinate c) {
    TileType type = maze.get_tile_type(c);
    if (type == LAVA) return 5;  // lava代价更高
    return 1;  // 其他可通行格子代价为1
}

// 深度优先搜索（栈实现，优先深搜）
std::vector<Coordinate> PathFinder::dfs() {
    std::vector<std::vector<bool>> visited(
        maze.get_rows(),
        std::vector<bool>(maze.get_cols(), false)
    );
    std::stack<Coordinate> s;
    std::unordered_map<int, Coordinate> parent; // 存储父节点，用于回溯路径
    std::vector<Coordinate> path;

    // 初始化：压入起点
    s.push(start);
    visited[start.y][start.x] = true;
    bool found = false;

    // 四个搜索方向（上、右、下、左，顺序影响DFS路径）
    int dx[] = { 0, 1, 0, -1 };
    int dy[] = { -1, 0, 1, 0 };

    while (!s.empty() && !found) {
        Coordinate current = s.top();
        s.pop();

        // 找到终点，终止搜索
        if (current.x == end.x && current.y == end.y) {
            found = true;
            break;
        }

        // 反向遍历方向（栈是LIFO，反向入栈保证搜索顺序一致）
        for (int i = 3; i >= 0; i--) {
            Coordinate next = {
                current.x + dx[i],
                current.y + dy[i]
            };

            if (is_valid(next, visited)) {
                visited[next.y][next.x] = true;
                parent[next.y * maze.get_cols() + next.x] = current; // 记录父节点
                s.push(next);
            }
        }
    }

    // 回溯重建路径（从终点到起点，再反转）
    if (found) {
        Coordinate curr = end;
        while (!(curr.x == start.x && curr.y == start.y)) {
            path.push_back(curr);
            // 通过哈希表查找父节点（key = y * 列数 + x）
            curr = parent[curr.y * maze.get_cols() + curr.x];
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end()); // 反转后得到 起点→终点 路径
    }

    return path;
}

// 广度优先搜索（队列实现，最短路径）
std::vector<Coordinate> PathFinder::bfs() {
    std::vector<std::vector<bool>> visited(
        maze.get_rows(),
        std::vector<bool>(maze.get_cols(), false)
    );
    std::queue<Coordinate> q;
    std::unordered_map<int, Coordinate> parent;
    std::vector<Coordinate> path;

    // 初始化：入队起点
    q.push(start);
    visited[start.y][start.x] = true;
    bool found = false;

    // 四个搜索方向（上、右、下、左，BFS顺序不影响最短路径结果）
    int dx[] = { 0, 1, 0, -1 };
    int dy[] = { -1, 0, 1, 0 };

    while (!q.empty() && !found) {
        Coordinate current = q.front();
        q.pop();

        // 找到终点，终止搜索
        if (current.x == end.x && current.y == end.y) {
            found = true;
            break;
        }

        // 遍历所有方向
        for (int i = 0; i < 4; i++) {
            Coordinate next = {
                current.x + dx[i],
                current.y + dy[i]
            };

            if (is_valid(next, visited)) {
                visited[next.y][next.x] = true;
                parent[next.y * maze.get_cols() + next.x] = current;
                q.push(next);
            }
        }
    }

    // 回溯重建路径
    if (found) {
        Coordinate curr = end;
        while (!(curr.x == start.x && curr.y == start.y)) {
            path.push_back(curr);
            curr = parent[curr.y * maze.get_cols() + curr.x];
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
    }

    return path;
}
std::vector<Coordinate> PathFinder::dijkstra() {
    int rows = maze.get_rows();
    int cols = maze.get_cols();

    // 记录到每个节点的最短距离
    std::vector<std::vector<int>> dist(rows, std::vector<int>(cols, INT_MAX));
    // 记录访问状态
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    // 记录路径
    std::unordered_map<int, Coordinate> parent;

    struct ComparePriority {
        bool operator()(const std::pair<int, Coordinate>& a, const std::pair<int, Coordinate>& b) const {
            // 优先按距离升序（小顶堆），距离相同则按坐标排序
            if (a.first != b.first) {
                return a.first > b.first;
            }
            if (a.second.x != b.second.x) {
                return a.second.x > b.second.x;
            }
            return a.second.y > b.second.y;
        }
    };

    std::priority_queue<std::pair<int, Coordinate>,
        std::vector<std::pair<int, Coordinate>>,
        ComparePriority> pq;

    // 初始化起点
    dist[start.y][start.x] = 0;
    pq.push({ 0, start });

    // 四个方向
    int dx[] = { 0, 1, 0, -1 };
    int dy[] = { -1, 0, 1, 0 };

    bool found = false;

    while (!pq.empty() && !found) {
        std::pair<int, Coordinate> top_elem = pq.top();
        pq.pop();
        int current_dist = top_elem.first;
        Coordinate current = top_elem.second;

        // 如果到达终点，结束搜索
        if (current.x == end.x && current.y == end.y) {
            found = true;
            break;
        }

        // 如果已经访问过，跳过
        if (visited[current.y][current.x]) continue;
        visited[current.y][current.x] = true;

        // 探索四个方向
        for (int i = 0; i < 4; i++) {
            Coordinate next = {
                current.x + dx[i],
                current.y + dy[i]
            };

            // 检查是否有效且未访问
            if (next.x >= 0 && next.x < cols && next.y >= 0 && next.y < rows &&
                maze.get_tile_type(next) != WALL && !visited[next.y][next.x]) {

                // 计算新距离
                int new_dist = current_dist + get_cost(next);

                // 如果找到更短的路径
                if (new_dist < dist[next.y][next.x]) {
                    dist[next.y][next.x] = new_dist;
                    parent[next.y * cols + next.x] = current;
                    pq.push({ new_dist, next });
                }
            }
        }
    }

    // 重建路径
    std::vector<Coordinate> path;
    if (found) {
        Coordinate curr = end;
        while (!(curr.x == start.x && curr.y == start.y)) {
            path.push_back(curr);
            // 检查是否存在父节点（防止迷宫无解时崩溃）
            int key = curr.y * cols + curr.x;
            if (parent.find(key) == parent.end()) {
                path.clear(); // 无有效路径，清空
                break;
            }
            curr = parent[key];
        }
        if (!path.empty()) { // 只有找到完整路径才添加起点并反转
            path.push_back(start);
            std::reverse(path.begin(), path.end());
        }
    }

    return path;
}
std::vector<Coordinate> PathFinder::dijkstra_with_one_lava() {
    int rows = maze.get_rows();
    int cols = maze.get_cols();

    // 距离数组，第一维是坐标，第二维是是否使用了熔岩
    std::vector<std::vector<std::vector<int>>> dist(
        rows, std::vector<std::vector<int>>(
            cols, std::vector<int>(2, INT_MAX)
        )
    );

    // 访问状态
    std::vector<std::vector<std::vector<bool>>> visited(
        rows, std::vector<std::vector<bool>>(
            cols, std::vector<bool>(2, false)
        )
    );

    // 记录路径的父节点（包括坐标和是否使用熔岩的状态）
    struct Node {
        Coordinate coord;
        bool used_lava;
    };

    std::unordered_map<std::string, Node> parent;

    // 自定义比较函数用于优先队列
    struct Compare {
        bool operator()(const std::tuple<int, Coordinate, bool>& a,
            const std::tuple<int, Coordinate, bool>& b) const {
            return std::get<0>(a) > std::get<0>(b);
        }
    };

    std::priority_queue<std::tuple<int, Coordinate, bool>,
        std::vector<std::tuple<int, Coordinate, bool>>,
        Compare> pq;

    // 初始化起点，没有使用熔岩
    dist[start.y][start.x][0] = 0;
    pq.push({ 0, start, false });

    // 四个方向
    int dx[] = { 0, 1, 0, -1 };
    int dy[] = { -1, 0, 1, 0 };

    bool found = false;
    bool final_used_lava = false;
    Coordinate final_coord = start;

    while (!pq.empty() && !found) {
        auto [current_dist, current, used_lava] = pq.top();
        pq.pop();

        // 如果已经访问过，跳过
        if (visited[current.y][current.x][used_lava ? 1 : 0]) continue;
        visited[current.y][current.x][used_lava ? 1 : 0] = true;

        // 如果到达终点
        if (current.x == end.x && current.y == end.y) {
            found = true;
            final_coord = current;
            final_used_lava = used_lava;
            break;
        }

        // 探索四个方向
        for (int i = 0; i < 4; i++) {
            Coordinate next = { current.x + dx[i], current.y + dy[i] };

            // 检查是否越界或是墙壁
            if (next.x < 0 || next.x >= cols || next.y < 0 || next.y >= rows) continue;
            if (maze.get_tile_type(next) == WALL) continue;

            // 计算到达下一个节点的成本
            bool next_used_lava = used_lava;
            int cost = 1;  // 默认成本

            TileType next_type = maze.get_tile_type(next);
            if (next_type == GRASS) {
                cost = 3;  // 草地成本高
            }
            else if (next_type == LAVA) {
                if (!used_lava) {
                    cost = 1;  // 允许踩一次熔岩
                    next_used_lava = true;
                }
                else {
                    continue;  // 已经踩过熔岩，不能再踩
                }
            }

            // 计算新距离
            int new_dist = current_dist + cost;
            int lava_state = next_used_lava ? 1 : 0;

            if (new_dist < dist[next.y][next.x][lava_state]) {
                dist[next.y][next.x][lava_state] = new_dist;

                // 记录父节点
                std::string key = std::to_string(next.y * cols + next.x) + "_" +
                    (next_used_lava ? "1" : "0");
                parent[key] = { current, used_lava };

                pq.push({ new_dist, next, next_used_lava });
            }
        }
    }

    // 重建路径
    std::vector<Coordinate> path;
    if (found) {
        Coordinate curr = final_coord;
        bool curr_used_lava = final_used_lava;

        while (!(curr.x == start.x && curr.y == start.y)) {
            path.push_back(curr);

            std::string key = std::to_string(curr.y * cols + curr.x) + "_" +
                (curr_used_lava ? "1" : "0");

            if (parent.find(key) != parent.end()) {
                Node prev = parent[key];
                curr = prev.coord;
                curr_used_lava = prev.used_lava;
            }
            else {
                break;
            }
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
    }

    return path;
}