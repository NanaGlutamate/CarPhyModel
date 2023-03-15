#pragma once

#include "../tools/constant.hpp"
#include "../tools/datastructure.hpp"
#include <cmath>
#include <tuple>

namespace carphymodel {

struct IntersectionInfo {
    double depthMin, depthMax;
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

inline constexpr IntersectionInfo NotHitInfo{0, 0, IntersectionInfo::HIT_SURFACE::NOT_HIT};

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
                tmp = static_cast<IntersectionInfo::HIT_SURFACE>(i * 2 + (d1 > d2) ? 1 : 0);
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

inline IntersectionInfo rayCollision(const Vector3 &dir, const Vector3 &pos, const Block &size,
                                     const Coordinate &coordinate) {
    auto tmp = lineCollision(dir, pos, size, coordinate);
    if (tmp.depthMin < 0) {
        return NotHitInfo;
    }
    return tmp;
}

inline IntersectionInfo segmentCollision(const Vector3 &pos1, const Vector3 &pos2, const Block &size,
                                         const Coordinate &coordinate) {
    auto dir = pos2 - pos1;
    double length = dir.norm();
    auto tmp = lineCollision(dir, pos1, size, coordinate);
    if (tmp.depthMin < 0 || tmp.depthMax > length) {
        return NotHitInfo;
    }
    return tmp;
}

// distance between Vector3 and Block
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