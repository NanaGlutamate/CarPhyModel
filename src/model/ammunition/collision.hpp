#pragma once

#include <array>
#include <cmath>
#include <tuple>

#include "../tools/constant.hpp"
#include "../tools/datastructure.hpp"

namespace carphymodel {

/// @brief collision result
struct IntersectionInfo {
    /// @brief depth range of collision, use dir.norm() as unit
    double minDepth, maxDepth;
    enum class HIT_SURFACE {
        FRONT = 0,
        BACK = 1,
        RIGHT = 2,
        LEFT = 3,
        BOTTOM = 4,
        TOP = 5,
        NOT_HIT = 6,
    } hitSurface;
    bool isCollision() { return hitSurface != HIT_SURFACE::NOT_HIT; }
};

/// @brief not hit info
inline constexpr IntersectionInfo NotHitInfo{0, 0, IntersectionInfo::HIT_SURFACE::NOT_HIT};

/**
 * @brief get armor thickness of givin surface of block
 *
 * @param pm armor block
 * @param surface surface of block
 * @return double
 */
inline constexpr double getSideArmor(const ProtectionModel &pm, IntersectionInfo::HIT_SURFACE surface) {
    return std::array<double, 6>{pm.armor_front, pm.armor_back,   pm.armor_side,
                                 pm.armor_side,  pm.armor_bottom, pm.armor_top}[static_cast<unsigned int>(surface)];
}

/// @brief unit normal vector of 6 direction
inline constexpr std::array<Vector3, 6> directionVector{
    Vector3{1, 0, 0}, Vector3{-1, 0, 0}, Vector3{0, 1, 0}, Vector3{0, -1, 0}, Vector3{0, 0, 1}, Vector3{0, 0, -1},
};

/**
 * @brief get collision result of a line with a block
 *
 * @param dir direction of the line
 * @param pos start position of the line
 * @param size block size
 * @param coordinate base coordinate of the block
 * @return IntersectionInfo collision result
 */
inline IntersectionInfo lineCollision(const Vector3 &dir, const Vector3 &pos, const Block &size,
                                      const Coordinate &coordinate) {
    const Vector3 dir_local = coordinate.directionWorldToBody(dir);
    const Vector3 pos_local = coordinate.positionWorldToBody(pos);
    IntersectionInfo::HIT_SURFACE tmp = IntersectionInfo::HIT_SURFACE::NOT_HIT;
    double depth_min = -INF_BIG, depth_max = INF_BIG;
    // Slabs method
    for (size_t i = 0; i < 3; ++i) {
        if (fabs(dir_local[i]) < INF_SMALL) {
            if (pos_local[i] <= -size[i] / 2 || pos_local[i] >= size[i] / 2)
                return NotHitInfo;
        } else {
            const double d1 = (size[i] / 2 - pos_local[i]) / dir_local[i];
            const double d2 = (-size[i] / 2 - pos_local[i]) / dir_local[i];
            const double d_min_tmp = fmin(d1, d2), d_max_tmp = fmax(d1, d2);
            if (d_min_tmp > depth_min) {
                depth_min = d_min_tmp;
                tmp = static_cast<IntersectionInfo::HIT_SURFACE>(i * 2 + ((d1 > d2) ? 1 : 0));
            }
            depth_max = fmin(depth_max, d_max_tmp);
        }
    }
    if (depth_max > depth_min) {
        return IntersectionInfo{depth_min, depth_max, tmp};
    } else {
        return NotHitInfo;
    }
}

/**
 * @brief get collision result of a ray with a block
 *
 * @param dir direction of the ray
 * @param pos start position of the ray
 * @param size block size
 * @param coordinate base coordinate of the block
 * @return IntersectionInfo collision result
 */
inline IntersectionInfo rayCollision(const Vector3 &dir, const Vector3 &pos, const Block &size,
                                     const Coordinate &coordinate) {
    auto tmp = lineCollision(dir, pos, size, coordinate);
    tmp.minDepth = fmax(tmp.minDepth, 0.);
    if (tmp.maxDepth < tmp.minDepth) {
        return NotHitInfo;
    }
    return tmp;
}

/**
 * @brief get collision result of a segment with a block
 *
 * @param pos1 start position of the segment
 * @param pos2 end position of the segment
 * @param size block size
 * @param coordinate base coordinate of the block
 * @return IntersectionInfo collision result, dir.norm() is 1
 */
inline IntersectionInfo segmentCollision(const Vector3 &pos1, const Vector3 &pos2, const Block &size,
                                         const Coordinate &coordinate) {
    auto dir = pos2 - pos1;
    double length = dir.norm();
    auto tmp = lineCollision(dir / length, pos1, size, coordinate);
    tmp.minDepth = fmax(tmp.minDepth, 0.);
    tmp.maxDepth = fmin(tmp.maxDepth, length);
    if (tmp.maxDepth < tmp.minDepth) {
        return NotHitInfo;
    }
    return tmp;
}

/**
 * @brief get distance between point and Block
 *
 * @param pos point
 * @param size block size
 * @param coordinate base coordinate of the block
 * @return double distance
 */
inline double pointBlockDistance(const Vector3 &pos, const Block &size, const Coordinate &coordinate) {
    const Vector3 pos_local = coordinate.positionWorldToBody(pos);
    double r2 = 0.;
    for (size_t i = 0; i < 3; ++i) {
        if (pos_local[i] < -size[i] / 2.) {
            r2 += pow(pos_local[i] + size[i] / 2., 2);
        } else if (pos_local[i] > size[i] / 2.) {
            r2 += pow(pos_local[i] - size[i] / 2., 2);
        }
    }
    return sqrt(r2);
}

} // namespace carphymodel