//
// Created by chh3213 on 2022/11/28.
//

#include "Astar.h"
#include <queue>
#include <set>
#include "../environment.h"
#include <list>
#include <chrono>

struct TimeCounter {
    size_t& tar;
    std::chrono::high_resolution_clock::time_point start_time;
    TimeCounter(size_t& tar) : tar(tar) {
        start_time = std::chrono::high_resolution_clock::now();
    }
    ~TimeCounter() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        tar += duration;
    }
};

/**
 * @brief use Bresenham algorithm to draw line with callback function
 *
 * @tparam Func
 * @param a first pos
 * @param b second pos
 * @param f call back function, return false to stop
 * @return requires
 */

template <std::invocable<size_t, size_t> Func>
    requires std::is_convertible_v<std::invoke_result_t<Func, int, int>, bool>
void BresenhamLine(int x1, int y1, int x2, int y2, Func&& f) {
    TimeCounter tc{Astar::line_time};
    Astar::linkedchecked++;
    int dx = x2 - x1, dy = y2 - y1;
    int s1 = (dx >= 0 ? 1 : -1), s2 = (dy >= 0 ? 1 : -1);
    dx = abs(dx);
    dy = abs(dy);
    int xbase = x1, ybase = y1;
    if (!f(xbase, ybase)) {
        return;
    }
    if (dx >= dy) {
        int e = (dy << 1) - dx, deta1 = dy << 1, deta2 = (dy - dx) << 1;
        while (xbase != x2) {
            if (e >= 0) {
                xbase += s1;
                // if (!f(xbase, ybase)) {
                //     return;
                // }
                ybase += s2;
                e += deta2;
            } else {
                xbase += s1;
                e += deta1;
            }
            if (!f(xbase, ybase)) {
                return;
            }
        }
    } else {
        int e = (dx << 1) - dy, deta1 = dx << 1, deta2 = (dx - dy) << 1;
        while (ybase != y2) {
            if (e >= 0) {
                ybase += s2;
                // if (!f(xbase, ybase)) {
                //     return;
                // }
                xbase += s1;
                e += deta2;
            } else {
                ybase += s2;
                e += deta1;
            }
            if (!f(xbase, ybase)) {
                return;
            }
        }
    }
}

Astar::Node::Node(double x, double y, float cost, double parentIndex)
    : x(x), y(y), cost(cost), parent_index(parentIndex) {}

Astar::Astar(double resolution, double robotRadius) : resolution(resolution), robot_radius(robotRadius) {}

/**
 * 得到障碍物信息图，有障碍物的地方标记为true，没有标记为false
 * @param ox 障碍物x坐标集合
 * @param oy 障碍物y坐标集合
 */
void Astar::calObstacleMap(const std::vector<double>& ox, const std::vector<double>& oy) {
    //TimeCounter tc{Astar::calfinalpath_time};
    min_x = round(*min_element(ox.begin(), ox.end()));
    min_y = round(*min_element(oy.begin(), oy.end()));
    max_x = round(*max_element(ox.begin(), ox.end()));
    max_y = round(*max_element(oy.begin(), oy.end()));

    std::cout << "min_x:" << min_x << "   min_y:" << min_y << "  max_x:" << max_x << "  max_y:" << max_y << std::endl;

    x_width = round((max_x - min_x) / resolution);
    y_width = round((max_y - min_y) / resolution);
    std::cout << "x_width:" << x_width << "  y_width:" << y_width << std::endl;

    obstacle_map = std::vector<std::vector<bool>>(x_width + 1, std::vector<bool>(y_width + 1, false));//这里的std::vector不好？

    for (double i = 0; i < ox.size(); i++) {
        obstacle_map[calXyIndex(ox[i], min_x)][calXyIndex(oy[i], min_y)] = true;
    }

    /*for (double i = 0; i < x_width; i++) {
        double x = calPosition(i, min_x);
        for (double j = 0; j < y_width; j++) {
            double y = calPosition(j, min_y);
            for (double k = 0; k < ox.size(); k++) {
                double d = sqrt(pow(ox[k] - x, 2) + pow(oy[k] - y, 2));
                if (d <= robot_radius) {
                    obstacle_map[i][j] = true;
                    break;
                }
            }
        }
    }*/
}

/**
 * 计算栅格在地图中的位置
 * @param index
 * @param minp
 * @return
 */
double Astar::calPosition(double index, double minp) {
    double pos = index * resolution + minp;
    return pos;
}

/**
 * 标记移动代价
 * @return
 */
std::vector<std::vector<double>> Astar::getMotionModel() {
    // x,y,cost
    motion = {{1, 0, 1},         {0, 1, 1},        {-1, 0, 1},       {0, -1, 1},
              {-1, -1, sqrt(2)}, {-1, 1, sqrt(2)}, {1, -1, sqrt(2)}, {1, 1, sqrt(2)}/*,
              {2, 1, sqrt(5)},   {2, -1, sqrt(5)}, {-2, 1, sqrt(5)}, {-2, -1, sqrt(5)},
              {1, 2, sqrt(5)},   {1, -2, sqrt(5)}, {-1, 2, sqrt(5)}, {-1, -2, sqrt(5)}*/};
    return motion;
}

/**
 * 计算起点终点的栅格索引
 * @param position
 * @param minp
 * @return
 */
double Astar::calXyIndex(double position, double minp) { return round((position - minp) / resolution); }

/**
 * 计算栅格索引
 * @param node
 * @return
 */
double Astar::calIndex(Astar::Node* node) {
    // cout<<node->x<<","<<node->y<<endl;
    //return (node->y - min_y) * x_width + (node->x - min_x);//change by wsb
    return node->y * x_width + node->x;
}

/**
 * 判断节点是否有效，即是否超出边界和碰到障碍物
 * @param node
 * @return
 */
bool Astar::verifyNode(Astar::Node* node) {
    double px = calPosition(node->x, min_x);
    double py = calPosition(node->y, min_y);
    if (px < min_x)
        return false;
    if (py < min_y)
        return false;
    if (px >= max_x)
        return false;
    if (py >= max_y)
        return false;
    if (obstacle_map[node->x][node->y])
        return false;
    return true;
}

/**
 * 计算路径，便于画图
 * @param goal_node
 * @param closed_set
 * @return
 */
std::vector<carphymodel::Vector3> Astar::calFinalPath(Astar::Node* goal_node, std::map<double, Node*> closed_set) {
    std::vector<carphymodel::Vector3> rxy;
    rxy.push_back(carphymodel::Vector3(calPosition(goal_node->x, min_x), calPosition(goal_node->y, min_y), 0.0));
    /*std::vector<double> rx, ry;
    rx.push_back(calPosition(goal_node->x, min_x));
    ry.push_back(calPosition(goal_node->y, min_y));*/

    double parent_index = goal_node->parent_index;

    while (parent_index != -1) {
        Node* node = closed_set[parent_index];
        rxy.push_back(carphymodel::Vector3(calPosition(node->x, min_x), calPosition(node->y, min_y), 0.0));
        /*rx.push_back(calPosition(node->x, min_x));
        ry.push_back(calPosition(node->y, min_y));*/

        parent_index = node->parent_index;
    }
    return rxy;
}

template <typename Ty>
struct Mempool {
    std::deque<Ty> pool;
    template <typename... Args>
    Ty* emplace(Args&&... args) {
        pool.emplace_back(std::forward<Args>(args)...);
        return &pool.back();
    }
};

/**
 * 规划
 * @param start 起点
 * @param goal 终点
 * @return 规划后的路径
 */
std::vector<carphymodel::Vector3> Astar::planning(std::vector<double> start, std::vector<double> goal) {
    double sx = start[0], sy = start[1];
    double gx = goal[0], gy = goal[1];

    TimeCounter tc{Astar::total_time};
    // use memory pool to avoid memory leak writed by WSB.
    Mempool<Node> mempool{};

    Node* start_node = mempool.emplace(Node(calXyIndex(sx, min_x), calXyIndex(sy, min_y), 0.0, -1));
    Node* goal_node = mempool.emplace(Node(calXyIndex(gx, min_x), calXyIndex(gy, min_y), 0.0, -1));

    std::map<double, Node*> closed_set;
    auto cmp = [this, goal_node, &closed_set](Node* a, Node* b) {
        return a->cost + calHeuristic(a, goal_node, closed_set) > b->cost + calHeuristic(b, goal_node, closed_set);
    };
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> open_set{cmp};
    std::set<double> open_set_id;
    // 将起点加入到open set
    open_set.push(start_node);
    open_set_id.insert(calIndex(start_node));
    // open_set[calIndex(start_node)] = start_node;
    // cout<<calIndex(start_node)<<endl;
    Node* current;
    while (true) {
        /* double c_id = numeric_limits<double>::max();
         double cost = numeric_limits<double>::max();*/
        // 计算代价最小的节点,与dijkstra代码维一不同的地方，即启发函数的计算不同，其他都一样
        // for (auto it = open_set.begin(); it != open_set.end(); it++) {
        //     double now_cost = it->second->cost + calHeuristic(goal_node, it->second);
        //     if (now_cost < cost) {
        //         cost = now_cost;
        //         c_id = it->first;
        //     }
        //     //cout<<it->first<<","<<it->second->cost<<endl;
        // }
        if (open_set.empty()) {
            std::cout << "No path found" << std::endl;
            return std::vector<carphymodel::Vector3>();
        }
        current = open_set.top();
        open_set.pop();
        if (open_set_id.find(calIndex(current)) ==
            open_set_id
                .end()) // 没有找到id：相同id，整体cost更小的id已经被erase了,这里如果查不到对应的id就pop，处理重复的节点
        {
            continue;
        } else {
            open_set_id.erase(calIndex(current));
        }

        double c_id = calIndex(current);//check
        Node *parent_node = (current->parent_index == -1)? nullptr : closed_set[current->parent_index];
        auto linked = [&, this](int x1, int y1, int x2, int y2) {
            bool ans = true;
            BresenhamLine(x1, y1, x2, y2, [&](int i, int j) {
                if (i < 0 || i >= x_width || j < 0 || j >= y_width) {
                    return ans = false;
                }
                return ans = !obstacle_map[i][j];
            });
            return ans;
        };
        auto distance = [&, this](int x1, int y1, int x2, int y2) {
            return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
        };

        // lazy theta*
        //if (parent_node && !linked(current->x, current->y, parent_node->x, parent_node->y))// 父节点和当前节点不连通
        //{
        //    double costtmp = 1e9;
        //    //遍历current的相邻节点，且必须满足在close_set中，是否满足linked关系，在满足的节点中再选择(cost + calHeuristic)最小的
        //    for (auto& move : motion)
        //    {
        //        int x = current->x + move[0];
        //        int y = current->y + move[1];
        //        if (closed_set.find(y * x_width + x) != closed_set.end() && linked(current->x, current->y, x, y))
        //        {
        //            Node* neighbornode = closed_set[y * x_width + x];
        //            if (costtmp > neighbornode->cost + distance(x, y, current->x, current->y))//calHeuristic(neighbornode, current, closed_set))//多个相等怎么办？
        //            {
        //                costtmp = neighbornode->cost + distance(x, y, current->x, current->y);
        //                current -> parent_index = calIndex(neighbornode);
        //                current->cost = costtmp;
        //            }
        //        }
        //    }
        //}
        parent_node = (current->parent_index == -1) ? nullptr : closed_set[current->parent_index];
        // plotGraph(current);//画图

        //find goal
        if (abs(current->x - goal_node->x) < EPS && abs(current->y - goal_node->y) < EPS) {
            std::cout << "Find goal" << std::endl;
            goal_node->parent_index = current->parent_index;
            goal_node->cost = current->cost;
            break;
        }

        // 将其加入到closed set
        closed_set[c_id] = current;
        // expand search grid based on motion model
        for (auto&& move : motion) {
            // cout<<move[0]<<move[1]<<move[2]<<endl;
            Node* node = mempool.emplace(Node(current->x + move[0], current->y + move[1], current->cost + move[2], c_id));
            double n_id = calIndex(node);

            if (closed_set.find(n_id) != closed_set.end())
                continue; // 如果已经在closed_set中了

            if (!verifyNode(node))
                continue; // 如果超出边界或者碰到障碍物了

            // 判断current和c_id的父节点能否直接相连——theta*
            if (parent_node && linked(node->x, node->y, parent_node->x, parent_node->y)) {
                node->cost = parent_node->cost + distance(node->x, node->y, parent_node->x, parent_node->y);
                node->parent_index = calIndex(parent_node);
            }

            //lazy theta*
            /*if (parent_node) {
                node->cost = parent_node->cost + distance(node->x, node->y, parent_node->x, parent_node->y);
                node->parent_index = calIndex(parent_node);
            }*/

            // 这里不判断cost，直接push
            open_set.push(node); // 疑问：相同的整体cost情况下，后加入的节点是先出还是后出？
            open_set_id.insert(n_id); // 可能有重复的节点，后边pop时候做处理
        }

        //std::cout << "closed_set_size:" << closed_set.size() << std::endl;
    }
    //plotGraph(current);
    return calFinalPath(goal_node, closed_set);
}

/**
 * 画图
 * @param current
 */
//void Astar::plotGraph(Astar::Node* current) {
//    // plt::clf();
//    plt::plot(ox, oy, ".k");
//    plt::plot(std::vector<double>{st[0]}, std::vector<double>{st[1]}, "og");
//    plt::plot(std::vector<double>{go[0]}, std::vector<double>{go[1]}, "xb");
//    plt::grid(true);
//    plt::plot(std::vector<double>{calPosition(current->x, min_x)}, std::vector<double>{calPosition(current->y, min_y)},
//              "xc");
//    plt::pause(0.001);
//}

void Astar::setSt(const std::vector<double>& st) { Astar::st = {st.begin(), st.end()}; } // double 变 size_t

void Astar::setGo(const std::vector<double>& go) { Astar::go = {go.begin(), go.end()}; } // double 变 size_t

void Astar::setOx(const std::vector<double>& ox) { Astar::ox = ox; }

void Astar::setOy(const std::vector<double>& oy) { Astar::oy = oy; }

/**
 * 启发函数计算，与dijkstra不同的地方，astar增加了对目标点的启发函数
 * @param n1 节点1
 * @param n2 节点2
 * @return
 */
// 默认n2是目标点
double Astar::calHeuristic(Astar::Node* n1, Astar::Node* n2, const std::map<double, Node*>& closed_set) {
    double w = 1.0; // 启发函数权重

    //double ob_num = 0; // 两层的n1与父节点直线连线上的障碍物个数，n1最多有一个坐标与父节点坐标相差2
    //Node* parent = closed_set.find(n1->parent_index)->second;
    //double dx = n1->x - parent->x;
    //double dy = n1->y - parent->y;
    //if (abs(dx) == 2) // 左右两侧
    //{
    //    if (obstacle_map[parent->x + dx / 2][parent->y])
    //        ob_num++;
    //    if (obstacle_map[parent->x + dx / 2][parent->y + dy])
    //        ob_num++;

    //} else if (abs(dy) == 2) // 上下两侧
    //{
    //    if (obstacle_map[parent->x][parent->y + dy / 2])
    //        ob_num++;
    //    if (obstacle_map[parent->x + dx][parent->y + dy / 2])
    //        ob_num++;
    //}

    //double distance = sqrt(pow(n1->x - parent->x, 2) + pow(n1->y - parent->y, 2));
    double m = 0;
    double n = 0;
    /*double t = abs((n1->x - n2->x) / (n1->y - n2->y));
    abs(parent->y - n1->y) == 2 && t >= 0.6667 ? m = ob_num, n = distance : m = 0, n = 0;
    abs(parent->x - n1->x) == 2 && t <= 1.4721 ? m = ob_num, n = distance : m = 0, n = 0;*/

    double d = w * (sqrt(pow(n1->x - n2->x, 2) + pow(n1->y - n2->y, 2)) + m - n); // 欧式距离+改进
    // double d = w * (abs(n1->x - n2->x) + abs(n1->y - n2->y));//曼哈顿距离
    // 对角距离
    /*double dx = abs(n1->x - n2->x);
    double dy = abs(n1->y - n2->y);
    double d = w * (dx + dy - min(dx, dy) * sqrt(2));*/
    return d;
}
