#pragma once

namespace carphymodel{

// gravity
constexpr inline double G = 9.8;
// PI
constexpr inline double PI = 3.1415926535897932384626433;
// inf_s
constexpr inline double INF_SMALL = 1e-10;
// inf_m
constexpr inline double INF_BIG = 1e+10;

constexpr inline double RAD2DEG(double rad){return rad * 180. / PI;}
constexpr inline double DEG2RAD(double deg){return deg * PI / 180.;}

}
