#pragma once

#include <tuple>
#include <cmath>
#include "constant.hpp"
#include "datastructure.hpp"

namespace carphymodel{

// array collision with AABB
std::tuple<double, bool> collision(
    const Vector3& dir,
    const Vector3& pos,
    const Block& size,
    const Coordinate& coordinate)
{
    const Vector3 dir_local = coordinate.directionWorldToBody(dir);
    const Vector3 pos_local = coordinate.positionWorldToBody(pos);
    double depth_min = 0, depth_max = INF_BIG;
    for(size_t i=0; i<3; ++i){
        if(fabs(dir_local[i]) < INF_SMALL){
            if(pos_local[i] < -size[i] / 2. || pos_local[i] > size[i] / 2.){return std::make_tuple(0., false);}
        }else{
            const double d1 = (size[i] / 2. - pos_local[i]) / dir_local[i];
            const double d2 = (-size[i] / 2. - pos_local[i]) / dir_local[i];
            depth_min = fmax(depth_min, fmin(d1, d2));
            depth_max = fmin(depth_max, fmax(d1, d2));
        }
    }
    return std::make_tuple(depth_min, depth_max > depth_min);
}

// distance between Vector3 and Block
double pointBlockDistance(
    const Vector3& pos, 
    const Block& size, 
    const Coordinate& coordinate){
    const Vector3 pos_local = coordinate.positionWorldToBody(pos);
    double r2 = 0.;
    for(size_t i=0; i<3; ++i){
        if(pos_local[i] < -size[i] / 2.)r2 += pow(pos_local[i] + size[i] / 2., 2);
        if(pos_local[i] > size[i] / 2.)r2 += pow(pos_local[i] - size[i] / 2., 2);
    }
    return sqrt(r2);
}

}