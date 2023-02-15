#pragma once

#include "vector3.hpp"

namespace carphymodel{

struct Coordinate{
    // 本坐标系原点在基坐标系中的位置
    Vector3 position;
    // 与基坐标系方向相同、原点与本坐标系重合的坐标系中基矢到本坐标系基矢的旋转四元数的前三个分量
    // 由于旋转四元数有模长为1的约束，若直接保存四元数，则为了避免迭代过程中的累计误差，需要在每次迭代中进行归一化
    // 若只保存旋转四元数的x, y, z三个分量，则只需要在迭代时计算w分量
    Vector3 attitude;

    //! 坐标转换
    //! @param p 基坐标系中的坐标
    //! @return 基坐标系中的坐标表示的点在本坐标系中的坐标
    Vector3 positionWorldToBody(const Vector3& p) const{
        auto inverse = Quaternion::fromCompressedQuaternion(attitude).inverse();
        auto p1 = inverse.rotate(Quaternion::fromVector(p - position));
        return Vector3(p1.x, p1.y, p1.z);
    };

    //! 坐标转换
    //! @param p 本坐标系中的坐标
    //! @return 本坐标系中的坐标表示的点在基坐标系中的坐标
    Vector3 positionBodyToWorld(const Vector3& p) const{
        Vector3 p1 = attitude.rotate(p);
        p1 += position;
        return p1;
    };

    //! 方向转换
    //! @param dir 基坐标系中的方向向量坐标
    //! @return 基坐标系中的方向向量坐标表示的点在本坐标系中的坐标
    Vector3 directionWorldToBody(const Vector3& dir) const{
        auto inverse = Quaternion::fromCompressedQuaternion(attitude).inverse();
        auto dir1 = inverse.rotate(Quaternion::fromVector(dir));
        return Vector3(dir1.x, dir1.y, dir1.z);
    };

    //! 方向转换
    //! @param dir 本坐标系中的方向向量坐标
    //! @return 本坐标系中的方向向量坐标表示的点在基坐标系中的坐标
    Vector3 directionBodyToWorld(const Vector3& dir) const{
        Vector3 dir1 = attitude.rotate(dir);
        return dir1;
    };
};

};