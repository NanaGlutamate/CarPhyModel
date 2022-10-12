#pragma once

#include <math.h>
#include "dataType.hpp"

namespace radar {

    constexpr double Pi{ 3.14159265358979323846 };
    //constexpr double DEG2RAD{ 0.017453292519943295 };
    //constexpr double RAD2DEG{ 57.29577951308232087 };

    constexpr double a = 6378137.0;            /* (WGS84)椭球长轴 (m) */
    constexpr double a_2 = a * a;           /* (WGS84)椭球长轴平方 (m) */
    constexpr double b =
        6356752.314245179;             /* (WGS84)椭球短轴 (m) */
    constexpr double b_2 = b * b;             /* (WGS84)椭球短轴平方 (m) */
    constexpr double f = (a - b) / a; /* (WGS84)椭球扁率 */
    constexpr double f_2 = (a - b) / b; /* (WGS84)椭球扁率 */
    constexpr double j_e2 = b_2 / a_2;
    constexpr double e2 = 1.0 - j_e2;;
    constexpr double e2_2 = (a_2 - b_2) / b;
    constexpr double tolerance = 1.e-12;;
    class CoorTrans {
    public:
        CoorTrans();
        CoorTrans(Vector3 given_ori_geod);
        virtual ~CoorTrans();
        //private:
        static Vector3 Origin_GEOC, Origin_RAD, Origin_GEOD;
    public:

        //站心坐标系转测地坐标系 站心坐标系用于卫星等高轨道模型
        static Vector3 ENU2GEOD(Vector3 enu_pos);

        //测地坐标系转站心坐标系
        static Vector3 GEOD2ENU(Vector3 geod_pos);

        //站心坐标系转地心坐标系
        static Vector3 ENU2GEOC(Vector3 enu_pos);

        //地心坐标系转站心坐标系
        static Vector3 GEOC2ENU(Vector3 geoc_pos);

        //地心坐标系转测地坐标系
        static Vector3 GEOD2GEOC(Vector3 geod_pos);

        //测地坐标系转地心坐标系
        static Vector3 GEOC2GEOD(Vector3 geoc_pos);

        //设置站心原点
        static void setOriginGEOD(Vector3 settingOrigin);

        //高度为水平高度差的测地到东北天坐标系转换 东北天坐标系用于近地面模型
        static Vector3 GEOD2ENS(Vector3 geod_pos);

        //高度为水平高度差的东北天坐标系到测地转换
        static Vector3 ENS2GEOD(Vector3 ens_pos);

        //高度为水平高度差的地心坐标系到东北天坐标系转换
        static Vector3 GEOC2ENS(Vector3 geod_pos);

        //高度为水平高度差的东北天坐标系到地心坐标系转换
        static Vector3 ENS2GEOC(Vector3 ens_pos);

        //北天东转经纬高
        static Vector3 NUE2GEOD(Vector3 nue_pos);

        //经纬高转北天东
        static Vector3 GEOD2NUE(Vector3 geod_pos);
    };

}