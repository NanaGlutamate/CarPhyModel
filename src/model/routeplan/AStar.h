#ifndef CHHROBOTICS_CPP_DIJKSTRA_H
#define CHHROBOTICS_CPP_DIJKSTRA_H

#include <iostream>
// #include <Eigen/Dense>
#include <algorithm>
#include <cmath>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "../tools/datastructure.hpp"

// using namespace Eigen;

#define EPS 1e-4
#define PI 3.14159265354

class Astar {
  public:
    struct Node {
        double x;
        double y;
        float cost;
        // Node* p_node;
        double parent_index;

        Node(double x, double y, float cost, double parentIndex);

        // bool operator<(const Node& node) const {
        //     return cost + calHeuristic(this, &node) < node.cost + calHeuristic(&node, );
        // }
        // bool operator>(const Node& node) const {
        //     return cost > node.cost;
        // }
    };

  private:
    double resolution; // 栅格大小
    double robot_radius;
    double min_x, min_y, max_x, max_y;           // 地图范围
    double x_width, y_width;                     // 长宽
    std::vector<std::vector<bool>> obstacle_map; // 障碍物地图
    std::vector<std::vector<double>> motion;     // 障碍物地图
    std::vector<double> st, go;
    std::vector<double> ox, oy;

  public:
    Astar(double resolution, double robotRadius);

    void calObstacleMap(const std::vector<double>& ox, const std::vector<double>& oy);

    double calPosition(double index, double minp);

    std::vector<std::vector<double>> getMotionModel();

    double calXyIndex(double position, double minp);

    double calIndex(Node* node);

    bool verifyNode(Node* node);

    std::vector<carphymodel::Vector3> calFinalPath(Node* goal_node, std::map<double, Node*> closed_set);

    std::vector<carphymodel::Vector3> planning(std::vector<double> start, std::vector<double> goal);

    double calHeuristic(Node* n1, Node* n2, const std::map<double, Node*>& closed_set);

    //void plotGraph(Node* current);

    void setSt(const std::vector<double>& st);

    void setGo(const std::vector<double>& go);

    void setOx(const std::vector<double>& ox);

    void setOy(const std::vector<double>& oy);
};
#endif // CHHROBOTICS_CPP_DIJKSTRA_H
