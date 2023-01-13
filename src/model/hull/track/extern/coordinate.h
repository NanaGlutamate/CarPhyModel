#ifndef __COORDINATE_H__
#define __COORDINATE_H__

#include "vector3.h"

namespace externModel::track{

struct Coordinate{
    // 新坐标系原点在基坐标系中的位置
    Vector33 position;
    // 与基坐标系方向相同、原点与新坐标系重合的坐标系中基矢到新坐标系基矢的旋转四元数的前三个分量
    // 由于旋转四元数有模长为1的约束，若直接保存四元数，则为了避免迭代过程中的累计误差，需要在每次迭代中进行归一化
    // 若只保存旋转四元数的x, y, z三个分量，则只需要在迭代时计算w分量
    Vector33 altitude;

    // Coordinate(const Vector33& position, const Vector33& altitude):position(position), altitude(altitude) {};
    // Coordinate(const Coordinate& c):position(c.position), altitude(c.altitude) {};
    // Coordinate(Coordinate&& c):position(c.position), altitude(c.altitude) {};

    //! 坐标转换
    //! @param p 基坐标系中的坐标
    //! @return 基坐标系中的坐标表示的点在新坐标系中的坐标
    inline Vector33 positionWorldToBody(const Vector33& p) const{
        auto inverse = Quaternion(altitude, true).inverse();
        auto p1 = inverse.rotate(Quaternion(p - position, false));
        return Vector33(p1.x, p1.y, p1.z);
    };

    //! 坐标转换
    //! @param p 新坐标系中的坐标
    //! @return 新坐标系中的坐标表示的点在基坐标系中的坐标
    inline Vector33 positionBodyToWorld(const Vector33& p) const{
        Vector33 p1 = altitude.rotate(p);
        p1 += position;
        return p1;
    };

    //! 方向转换
    //! @param dir 基坐标系中的方向向量坐标
    //! @return 基坐标系中的方向向量坐标表示的点在新坐标系中的坐标
    inline Vector33 directionWorldToBody(const Vector33& dir) const{
        auto inverse = Quaternion(altitude, true).inverse();
        auto dir1 = inverse.rotate(Quaternion(dir, false));
        return Vector33(dir1.x, dir1.y, dir1.z);
    };

    //! 方向转换
    //! @param dir 新坐标系中的方向向量坐标
    //! @return 新坐标系中的方向向量坐标表示的点在基坐标系中的坐标
    inline Vector33 directionBodyToWorld(const Vector33& dir) const{
        Vector33 dir1 = altitude.rotate(dir);
        return dir1;
    };
};

}

#endif