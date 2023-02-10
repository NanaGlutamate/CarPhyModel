// CoordTransform.cpp: implementation of the CCoordTransform class.
//
//////////////////////////////////////////////////////////////////////
#include "CoordTransform.hpp"

namespace externModel::radar {

    Vector3 CoorTrans::Origin_GEOC = Vector3{ -2812685.5132570122, 4998522.5083608516, 2786030.1928186743 };
    Vector3 CoorTrans::Origin_GEOD = Vector3{ 119.32, 26.07, 0 };
    Vector3 CoorTrans::Origin_RAD = Vector3{ 2.0825268634796341, 0.45500733599492171, 0 };

    CoorTrans::CoorTrans() {
        setOriginGEOD(Vector3{ 119.32, 26.07, 0 });
    }

    CoorTrans::CoorTrans(Vector3 given_ori_geod) {
        setOriginGEOD(given_ori_geod);
    }

    CoorTrans::~CoorTrans() {
    }

    void CoorTrans::setOriginGEOD(Vector3 setting_origin_geod) {
        Origin_GEOD = setting_origin_geod;
        Origin_GEOC = GEOD2GEOC(setting_origin_geod);
        Origin_RAD = Vector3(setting_origin_geod.x * DEG2RAD,
            setting_origin_geod.y * DEG2RAD, setting_origin_geod.z);
    }

    Vector3 CoorTrans::GEOD2GEOC(Vector3 geod_pos) {
        double lambda = geod_pos.x * DEG2RAD, fi = geod_pos.y * DEG2RAD, h = geod_pos.z;
        double sn = sin(fi), cs = cos(fi);
        double N = a / sqrt(1.0 - e2 * sn * sn);
        double X = (N + h) * cs * cos(lambda), Y = (N + h) * cs * sin(lambda),
            Z = (N * (1 - e2) + h) * sn;
        return Vector3(X, Y, Z);
    }

    Vector3 CoorTrans::GEOC2GEOD(Vector3 geoc_pos) {
        double step, x1, x1p2, y1, y1pr,
            f2, e22, f2R, X, Y, Z, R, A, B, C, tgEpocetno,
            tg, cs;

        Vector3 blh;

        X = geoc_pos.x;
        Y = geoc_pos.y;
        Z = geoc_pos.z;
        R = sqrt(X * X + Y * Y);

        if (0.0 == X && 0.0 == Y) {
            blh.x = 0.0;
            blh.y = Pi / 2 * RAD2DEG;
            blh.z = Z - b;
            return blh;
        }
        else if (0.0 == Z) {
            blh.x = atan2(Y, X) * RAD2DEG;
            blh.y = 0.0;
            blh.z = R - a;
            return blh;
        }

        else {
            f2 = a / b;
            e22 = a * f2 - b;
            f2R = f2 * R;

            A = 2. * (f2R + e22) / Z;
            B = 2. * (f2R - e22) / Z;
            C = 3. * A;

            tgEpocetno = f2 * Z / R;
            x1 = tgEpocetno / (1. + sqrt(1. + tgEpocetno * tgEpocetno));  // x=t=tan(E/2);
            x1p2 = x1 *
                x1; // �ų��˵�һ�ε�������Ϊ��������Ϊ������������.

         // Visual Studio
            y1 = x1 * (B + x1p2 * (A + x1)) - 1.;
            y1pr = x1p2 * (C + 4. * x1) + B;
            step = y1 / y1pr;

            // gcc - linux
            //y1 = x1;
            //y1pr = x1;
            //step = --((((y1 += A) *= x1p2) += B) *= x1) / ((((y1pr *= 4.) += C) *= x1p2) += B);

            x1 -= step;

            do {
                x1p2 = x1 *
                    x1; // ��һ�ε������ų����⣬��Ϊ��������Ϊ�����ų�����

             // Visual Studio
                y1 = x1 * (B + x1p2 * (A + x1)) - 1.;
                y1pr = x1p2 * (C + 4. * x1) + B;
                step = y1 / y1pr;

                // gcc - linux
                //y1 = y1pr = x1;
                //step = --((((y1 += A) *= x1p2) += B) *= x1) / ((((y1pr *= 4.) += C) *= x1p2) += B);

                x1 -= step;
            } while (step > tolerance);

            tg = (2. * x1) / (1. - x1 * x1) * f2;
            cs = 1. / sqrt(1. + tg * tg);
            //t = (1.-t)/(1.+t); // t = tg(pi/4 - E/2) -->  t = tg(E/2)
            blh.x = atan2(Y, X) * RAD2DEG;
            blh.y = atan(tg) * RAD2DEG;
            blh.z = (R - a * (1. - x1) / (1. + x1)) * cs + (Z - b) * tg * cs;

            return blh;
        }
    }

    Vector3 CoorTrans::GEOD2ENU(Vector3 geod_pos) {
        return GEOC2ENU(GEOD2GEOC(geod_pos));
    }

    Vector3 CoorTrans::ENU2GEOD(Vector3 enu_pos) {
        return GEOC2GEOD(ENU2GEOC(enu_pos));
    }

    Vector3 CoorTrans::ENU2GEOC(Vector3 enu_pos) {
        double e2g[] = {
            -sin(Origin_RAD.x),-sin(Origin_RAD.y) * cos(Origin_RAD.x),cos(Origin_RAD.y) * cos(Origin_RAD.x),
            cos(Origin_RAD.x),-sin(Origin_RAD.y) * sin(Origin_RAD.x),cos(Origin_RAD.y) * sin(Origin_RAD.x),
            0,cos(Origin_RAD.y),sin(Origin_RAD.y)
        };
        auto e2gm = Matrix3(e2g);
        return e2gm * enu_pos + Origin_GEOC;
    }

    Vector3 CoorTrans::GEOC2ENU(Vector3 geoc_pos) {
        double  g2e[] = {
            -sin(Origin_RAD.x), cos(Origin_RAD.x), 0,
            -sin(Origin_RAD.y) * cos(Origin_RAD.x), -sin(Origin_RAD.y) * sin(Origin_RAD.x), cos(Origin_RAD.y),
            cos(Origin_RAD.y) * cos(Origin_RAD.x), cos(Origin_RAD.y) * sin(Origin_RAD.x), sin(Origin_RAD.y)
        };
        auto g2em = Matrix3(g2e);

        return g2em * (geoc_pos - Origin_GEOC);
    }

    Vector3 CoorTrans::GEOD2ENS(Vector3 geod_pos) {
        double r = a / sqrt(1.0 - e2 * sin(Origin_RAD.y) * sin(Origin_RAD.y));
        double lon = (geod_pos.x * DEG2RAD - Origin_RAD.x) * r * cos(geod_pos.y * DEG2RAD);
        double lat = (geod_pos.y * DEG2RAD - Origin_RAD.y) * r;
        return Vector3{ lon, lat, geod_pos.z };
    }

    Vector3 CoorTrans::ENS2GEOD(Vector3 ens_pos) {
        double r = a / sqrt(1.0 - e2 * sin(Origin_RAD.y) * sin(Origin_RAD.y));
        double lat = Origin_RAD.y + ens_pos.y / r;
        double lon = Origin_RAD.x + ens_pos.x / (r * cos(lat));
        return Vector3{ lon * RAD2DEG, lat * RAD2DEG, ens_pos.z };
    }

    Vector3 CoorTrans::GEOC2ENS(Vector3 geoc_pos) {
        return GEOD2ENS(GEOC2GEOD(geoc_pos));
    }

    Vector3 CoorTrans::ENS2GEOC(Vector3 ens_pos) {
        return GEOD2GEOC(ENS2GEOD(ens_pos));
    }

    Vector3 CoorTrans::NUE2GEOD(Vector3 nue_pos) {
        Vector3 tmpPos(nue_pos.z, nue_pos.x, nue_pos.y);
        return ENS2GEOD(tmpPos);
    }

    Vector3 CoorTrans::GEOD2NUE(Vector3 geod_pos) {
        Vector3 tmpPos = GEOD2ENS(geod_pos);
        return Vector3(tmpPos.y, tmpPos.z, tmpPos.x);
    }

}