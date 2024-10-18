#pragma once

#include "framework/pattern.hpp"
#include "tools/vector3.hpp"
#include <memory>


namespace carphymodel {

class Environment {
  public:
    friend class EnvironmentInfoAgent;
    Environment() {
        // server ip | redis ip | rabbitmq ip
        // InitMiddleware("10.134.114.97:50052", nullptr, "amqp://guest:guest@10.134.114.97:5672");
    }
    Environment(const Environment&) = delete;
    Environment& operator=(const Environment&) = delete;
    ~Environment() {
        // CloseMiddleware();
     }
  private:
    virtual double getAltitude(const Vector3& pos) const { return 0.; }
    virtual double getSlope(const Vector3& pos, const Vector3& dir) const { return 0.; }
    virtual bool getIntervisibility(const Vector3& pos1, const Vector3& pos2) const {
        return pos1.z >= 0. && pos2.z >= 0.;
    }
    virtual std::vector<Vector3> getRoute(const Vector3& start, const Vector3& end) const {
        //实现逻辑从本地路径文件中读取路径点
        if (end.x == 1)
            return std::vector<Vector3>{
                {0.000000, 0.000000, 0.000000},      {-5.772000, -19.215904, 0.000000},
                {-14.097000, -42.131126, 0.000000},  {-16.539000, -51.790457, 0.000000},
                {-18.537000, -67.820836, 0.000000},  {-19.425000, -92.277441, 0.000000},
                {-19.314000, -140.368579, 0.000000}, {-17.538000, -144.581692, 0.000000},
                {-15.207000, -147.150663, 0.000000}, {-12.210000, -149.000322, 0.000000},
                {-9.435000, -149.616875, 0.000000},  {-1.554000, -150.130669, 0.000000},
                {0.333000, -150.130669, 0.000000},   {101.676000, -147.664457, 0.000000},
                {138.306000, -146.225833, 0.000000}, {157.287000, -147.458940, 0.000000},
                {170.607000, -149.000322, 0.000000}, {182.373000, -148.897563, 0.000000},
                {199.911000, -145.506522, 0.000000}, {360.639000, -101.217460, 0.000000},
                {428.682000, -83.440180, 0.000000},  {436.230000, -81.487762, 0.000000},
                {536.907000, -55.181499, 0.000000},  {572.094000, -44.700097, 0.000000},
                {641.358000, -22.812463, 0.000000},  {784.215000, 22.606945, 0.000000},
                {847.263000, 42.644920, 0.000000},   {888.666000, 49.632521, 0.000000},
                {886.557000, 35.451801, 0.000000},   {882.006000, 5.240701, 0.000000},
                {879.009000, -16.955209, 0.000000},  {877.233000, -31.238688, 0.000000},
                {861.360000, -135.230637, 0.000000}, {856.809000, -164.825184, 0.000000},
            };
        else if (end.x == 2)
            return std::vector<Vector3>{};
        else
            return std::vector<Vector3>{};
        // get routes from server
        // GetRoutePoints_return params = GetRoutePoints(113.5439372, 22.2180642, 113.5425177, 22.2252363);
        // printf("total points count: %d\n", params.r0);
        // for (int i = 0; i < params.r0; i++) {
        //     printf("lon-lat: (%lf, %lf)\n", params.r1[i], params.r2[i]);
        // }
        // FreePositionsPointer(params.r1, params.r2);
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

    //! @brief 获取两点间路径序列，序列必须包含起始点与终止点
    //!
    //! @param start 起始点
    //! @param end 终止点
    //! @return 中间路径点序列
    std::vector<Vector3> getRoute(const Vector3& start, const Vector3& end) { return env->getRoute(start, end); }

  private:
    inline static std::unique_ptr<Environment> env = std::make_unique<Environment>();
};

} // namespace carphymodel