#pragma once

#include "tools/metatools.hpp"
#include "tools/vector3.hpp"

namespace carphymodel {

namespace geoInfo{

struct Pos{};
struct Neg{};

struct NS{};
struct N : public NS, public Pos{};
struct S : public NS, public Neg{};

struct EW{};
struct E : public EW, public Pos{};
struct W : public EW, public Neg{};

struct DU{};
struct D : public DU, public Pos{};
struct U : public DU, public Neg{};

template <typename Base>
struct is_base_of_func{
    template <typename Ty>
    struct func{
        constexpr static inline bool value = std::is_base_of_v<Base, Ty>;
    };
};

#if __cplusplus >= 202002L
template<typename Side>
concept Direction = carphymodel::mymeta::is_in_list_v<Side, W, E, N, S, U, D>;

template<Direction X, Direction Y, Direction Z>
#else
template<typename X, typename Y, typename Z>
#endif
struct CoordinateOrder{
    using ns = typename mymeta::find_if<is_base_of_func<NS>::template func, void, X, Y, Z>::type;
    using ew = typename mymeta::find_if<is_base_of_func<EW>::template func, void, X, Y, Z>::type;
    using du = typename mymeta::find_if<is_base_of_func<DU>::template func, void, X, Y, Z>::type;
    static_assert(!std::is_same_v<void, ns>);
    static_assert(!std::is_same_v<void, ew>);
    static_assert(!std::is_same_v<void, du>);

    constexpr CoordinateOrder(double x, double y, double z){
        pos[mymeta::get_type_list_index<X, ns, ew, du>::value] = x * (std::is_base_of_v<Pos, X>? 1 : -1);
        pos[mymeta::get_type_list_index<Y, ns, ew, du>::value] = y * (std::is_base_of_v<Pos, Y>? 1 : -1);
        pos[mymeta::get_type_list_index<Z, ns, ew, du>::value] = z * (std::is_base_of_v<Pos, Z>? 1 : -1);
    };

    constexpr CoordinateOrder(const Vector3 v){
        pos[0] = v.x;
        pos[1] = v.y;
        pos[2] = v.z;
    };

    #if __cplusplus >= 202002L
    template<Direction _X, Direction _Y, Direction _Z>
    #else
    template<typename _X, typename _Y, typename _Z>
    #endif
    constexpr CoordinateOrder(const CoordinateOrder<_X, _Y, _Z>& c){
        pos[0] = c.pos[0];
        pos[1] = c.pos[1];
        pos[2] = c.pos[2];
    };

    #if __cplusplus >= 202002L
    template<Direction _X, Direction _Y, Direction _Z>
    #else
    template<typename _X, typename _Y, typename _Z>
    #endif
    constexpr CoordinateOrder& operator=(const CoordinateOrder<_X, _Y, _Z>& c){
        pos[0] = c.pos[0];
        pos[1] = c.pos[1];
        pos[2] = c.pos[2];
    };
    // 标准坐标轴顺序：北东地
    constexpr operator Vector3() const {
        return Vector3{pos[0], pos[1], pos[2]};
    };

    constexpr Vector3 getOriginalCoordinate() const {
        return Vector3{
            pos[mymeta::get_type_list_index<X, ns, ew, du>::value] * (std::is_base_of_v<Pos, X>? 1 : -1),
            pos[mymeta::get_type_list_index<Y, ns, ew, du>::value] * (std::is_base_of_v<Pos, Y>? 1 : -1),
            pos[mymeta::get_type_list_index<Z, ns, ew, du>::value] * (std::is_base_of_v<Pos, Z>? 1 : -1)
        };
    }
private:
    double pos[3];
};

constexpr double Pi{ 3.14159265358979323846 };
constexpr double DEG2RAD{ 0.017453292519943295 };
constexpr double RAD2DEG{ 57.29577951308232087 };
constexpr double a = 6378137.0;            /* (WGS84)椭球长轴 (m) */
constexpr double a_2 = a * a;           /* (WGS84)椭球长轴平方 (m) */
constexpr double b = 6356752.314245179;             /* (WGS84)椭球短轴 (m) */
constexpr double b_2 = b * b;             /* (WGS84)椭球短轴平方 (m) */
constexpr double f = (a - b) / a; /* (WGS84)椭球扁率 */
constexpr double f_2 = (a - b) / b; /* (WGS84)椭球扁率 */
constexpr double j_e2 = b_2 / a_2;
constexpr double e2 = 1.0 - j_e2;;
constexpr double e2_2 = (a_2 - b_2) / b;
constexpr double tolerance = 1.e-12;

}

struct UniversalPositionRepresentation{};

template<typename Output, typename Input, typename ...Info>
Output Transform(Input&& i, Info&&... info){
    UniversalPositionRepresentation tmp{std::forward<Input>(i)};
    return tmp.trans<Output>(std::forward<Info>(info)...);
}

}