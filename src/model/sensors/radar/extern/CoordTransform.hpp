#pragma once

#include <math.h>
#include "dataType.hpp"

namespace externModel::radar {

    constexpr double Pi{ 3.14159265358979323846 };
    //constexpr double DEG2RAD{ 0.017453292519943295 };
    //constexpr double RAD2DEG{ 57.29577951308232087 };

    constexpr double a = 6378137.0;            /* (WGS84)������ (m) */
    constexpr double a_2 = a * a;           /* (WGS84)������ƽ�� (m) */
    constexpr double b =
        6356752.314245179;             /* (WGS84)������� (m) */
    constexpr double b_2 = b * b;             /* (WGS84)�������ƽ�� (m) */
    constexpr double f = (a - b) / a; /* (WGS84)������� */
    constexpr double f_2 = (a - b) / b; /* (WGS84)������� */
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

        //վ������ϵת�������ϵ վ������ϵ�������ǵȸ߹��ģ��
        static Vector3 ENU2GEOD(Vector3 enu_pos);

        //�������ϵתվ������ϵ
        static Vector3 GEOD2ENU(Vector3 geod_pos);

        //վ������ϵת��������ϵ
        static Vector3 ENU2GEOC(Vector3 enu_pos);

        //��������ϵתվ������ϵ
        static Vector3 GEOC2ENU(Vector3 geoc_pos);

        //��������ϵת�������ϵ
        static Vector3 GEOD2GEOC(Vector3 geod_pos);

        //�������ϵת��������ϵ
        static Vector3 GEOC2GEOD(Vector3 geoc_pos);

        //����վ��ԭ��
        static void setOriginGEOD(Vector3 settingOrigin);

        //�߶�Ϊˮƽ�߶Ȳ�Ĳ�ص�����������ϵת�� ����������ϵ���ڽ�����ģ��
        static Vector3 GEOD2ENS(Vector3 geod_pos);

        //�߶�Ϊˮƽ�߶Ȳ�Ķ���������ϵ�����ת��
        static Vector3 ENS2GEOD(Vector3 ens_pos);

        //�߶�Ϊˮƽ�߶Ȳ�ĵ�������ϵ������������ϵת��
        static Vector3 GEOC2ENS(Vector3 geod_pos);

        //�߶�Ϊˮƽ�߶Ȳ�Ķ���������ϵ����������ϵת��
        static Vector3 ENS2GEOC(Vector3 ens_pos);

        //���춫ת��γ��
        static Vector3 NUE2GEOD(Vector3 nue_pos);

        //��γ��ת���춫
        static Vector3 GEOD2NUE(Vector3 geod_pos);
    };

}