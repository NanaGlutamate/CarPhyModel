#pragma once

#include "framework/pattern.hpp"
#include "tools/vector3.hpp"
#include <memory>

namespace carphymodel {

class Environment {
  public:
    friend class EnvironmentInfoAgent;
    Environment() {}
    Environment(const Environment&) = delete;
    Environment& operator=(const Environment&) = delete;

  private:
    virtual double getAltitude(const Vector3& pos) const { return 0.; }
    virtual double getSlope(const Vector3& pos, const Vector3& dir) const { return 0.; }
    virtual bool getIntervisibility(const Vector3& pos1, const Vector3& pos2) const {
        return pos1.z >= 0. && pos2.z >= 0.;
    }
    virtual std::optional<std::vector<Vector3>> getRoute(const Vector3& start, const Vector3& end) {
        return std::nullopt;
    }
};

class EnvironmentInfoAgent {
  public:
    EnvironmentInfoAgent() = default;

    static void changeEnvironmentSupplier(std::unique_ptr<Environment>&& n) { env = std::move(n); }

    //! @brief 采用北东地坐标系
    //! @param pos: 位置坐标(北东地)
    //! @return 位置在地表的投影相对某固定平面的高度(上为正)
    double getAltitude(const Vector3& pos) const { return env->getAltitude(pos); }

    //! @brief 采用北东地坐标系
    //! @param pos: 位置坐标(北东地)
    //! @param dir: 方向向量(北东地)
    //! @return 斜率(坡比，上坡为正)
    double getSlope(const Vector3& pos, const Vector3& dir) const { return env->getSlope(pos, dir); }

    //! @brief 采用北东地坐标系
    //! @param pos1: 位置坐标1(北东地)
    //! @param pos2: 位置坐标2(北东地)
    //! @return 是否可见
    bool getIntervisibility(const Vector3& pos1, const Vector3& pos2) const {
        return env->getIntervisibility(pos1, pos2);
    }

    //! @brief 获取两点间路径
    //! @param start 起始点
    //! @param end 终止点
    //! @return 中间路径点序列
    std::optional<std::vector<Vector3>> getRoute(const Vector3& start, const Vector3& end) { return std::nullopt; }

  private:
    inline static std::unique_ptr<Environment> env = std::make_unique<Environment>();
};

} // namespace carphymodel