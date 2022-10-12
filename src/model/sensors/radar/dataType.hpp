#pragma once

#include <cmath>

namespace radar {

    constexpr double PI{ 3.14159265358979323846 };
    constexpr double TWO_PI{ 6.28318530717958647692 };
    constexpr double DEG2RAD{ 0.017453292519943295 };
    constexpr double RAD2DEG{ 57.29577951308232087 };
    constexpr double C = 3e8;

    class Vector3 {
    public:
        Vector3() : x(0), y(0), z(0) {};
        virtual ~Vector3() = default;
        Vector3(double Valx, double Valy, double Valz) :x(Valx), y(Valy), z(Valz) {}
        Vector3(const Vector3& Value) :x(Value.x), y(Value.y), z(Value.z) {}

        double x, y, z;
        Vector3& operator=(const Vector3& b) {
            if (this == &b)
                return *this;
            x = b.x;
            y = b.y;
            z = b.z;
            return *this;
        }
        Vector3& operator=(Vector3&& b) {
            x = b.x;
            y = b.y;
            z = b.z;
            return *this;
        }
        double get_cosval(const Vector3& Value) {
            auto length = this->magnitude() * Value.magnitude();
            return (x * Value.x + y * Value.y + z * Value.z) / length;
        }
        double magnitude() const {
            return std::sqrt(x * x + y * y + z * z);
        }
        Vector3 cross(const Vector3& Val) {
            return Vector3(y * Val.z - z * Val.y, z * Val.x - x * Val.z, x * Val.y - y * Val.x);
        }
        Vector3 normalize() {
            double norm = this->magnitude();
            return Vector3{ x / norm, y / norm, z / norm };
        }

        Vector3 operator+(const Vector3& B) const {
            return Vector3(x + B.x, y + B.y, z + B.z);
        }
        Vector3 operator-(const Vector3& Val) const {
            return Vector3(x - Val.x, y - Val.y, z - Val.z);
        }
        Vector3 operator-() {
            x = -x;
            y = -y;
            z = -z;
            return *this;
        }
        Vector3 operator*(const double scalar) const {
            return Vector3(scalar * x, scalar * y, scalar * z);
        }
        Vector3 operator/(const double scalar) const {
            return operator*(1.0 / scalar);
        }
        bool operator==(const Vector3& equalv) const {
            return x == equalv.x && y == equalv.y && z == equalv.z;
        }
        bool operator!=(const Vector3& equalv) const {
            return x != equalv.x || y != equalv.y || z != equalv.z;
        }
        Vector3 operator*(const Vector3& V) const {
            return Vector3(y * V.z - z * V.y, z * V.x - x * V.z, x * V.y - y * V.x);
        }

        Vector3& operator+=(const Vector3& P) {
            x += P.x;
            y += P.y;
            z += P.z;
            return *this;
        }
        Vector3& operator-=(const Vector3& Sub) {
            x -= Sub.x;
            y -= Sub.y;
            z -= Sub.z;
            return *this;
        }
        Vector3& operator*=(const double scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }
        Vector3& operator/=(const double scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }
        double dot(const Vector3& V) const {
            return x * V.x + y * V.y + z * V.z;
        }

        Vector3 reciprocal(Vector3& ret) {
            return Vector3(1 / ret.x, 1 / ret.y, 1 / ret.z);
        }
    };


    class Matrix3 {
    public:
        double value[3][3];
        Matrix3() :value{ 0 } {}

        Matrix3(double ini) {
            for (auto& i : value) {
                for (auto j : i) {
                    j = ini;
                }
            }
        }
        Matrix3(double init[9]) {
            int i = 0;
            for (auto& j : value) {
                for (auto& k : j) {
                    k = init[i];
                    i++;
                }
            }
        }

        Matrix3(const Matrix3& init) {
            int i = 0, j = 0;
            for (; i < 3; i++) {
                for (; j < 3; j++) {
                    value[i][j] = init.value[i][j];
                }
            }
        }

        Matrix3(double angle, int x, int y, int z) {
            if (x != 0 && y == 0 && z == 0) {
                value[0][0] = 1.0;
                value[0][1] = 0.0;
                value[0][2] = 0.0;
                value[1][0] = 0.0;
                value[1][1] = cos(angle);
                value[1][2] = sin(angle);
                value[2][0] = 0.0;
                value[2][1] = -sin(angle);
                value[2][2] = cos(angle);
            }
            else if (x == 0 && y != 0 && z == 0) {
                value[0][0] = cos(angle);
                value[0][1] = 0.0;
                value[0][2] = -sin(angle);
                value[1][0] = 0.0;
                value[1][1] = 1.0;
                value[1][2] = 0.0;
                value[2][0] = sin(angle);
                value[2][1] = 0.0;
                value[2][2] = cos(angle);
            }
            else if (x == 0 && y == 0 && z != 0) {
                value[0][0] = cos(angle);
                value[0][1] = sin(angle);
                value[0][2] = 0.0;
                value[1][0] = -sin(angle);
                value[1][1] = cos(angle);
                value[1][2] = 0.0;
                value[2][0] = 0.0;
                value[2][1] = 0.0;
                value[2][2] = 1.0;
            }
            else {
                for (auto& i : value) {
                    for (auto j : i) {
                        j = 0;
                    }
                }
            }
        }

        Matrix3 T() {
            double tmplist[] = {
                value[0][0], value[1][0], value[2][0],
                value[0][1], value[1][1], value[2][1],
                value[0][2], value[1][2], value[2][2],
            };
            return Matrix3(tmplist);
        }

        Matrix3& operator=(const Matrix3 M) {
            if (&M == this) {
                return *this;
            }
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    value[i][j] = M.value[i][j];
                }
            }
            return *this;
        }

        Vector3 operator*(const Vector3& V) {
            return Vector3(V.x * value[0][0] + V.y * value[0][1] + V.z * value[0][2],
                V.x * value[1][0] + V.y * value[1][1] + V.z * value[1][2],
                V.x * value[2][0] + V.y * value[2][1] + V.z * value[2][2]);
        }

        Matrix3 operator*(const Matrix3& M) {
            double M_v[] = {
                value[0][0] * M.value[0][0] + value[0][1] * M.value[1][0] + value[0][2] * M.value[2][0],
                value[0][0] * M.value[0][1] + value[0][1] * M.value[1][1] + value[0][2] * M.value[2][1],
                value[0][0] * M.value[0][2] + value[0][1] * M.value[1][2] + value[0][2] * M.value[2][2],
                value[1][0] * M.value[0][0] + value[1][1] * M.value[1][0] + value[1][2] * M.value[2][0],
                value[1][0] * M.value[0][1] + value[1][1] * M.value[1][1] + value[1][2] * M.value[2][1],
                value[1][0] * M.value[0][2] + value[1][1] * M.value[1][2] + value[1][2] * M.value[2][2],
                value[2][0] * M.value[0][0] + value[2][1] * M.value[1][0] + value[2][2] * M.value[2][0],
                value[2][0] * M.value[0][1] + value[2][1] * M.value[1][1] + value[2][2] * M.value[2][1],
                value[2][0] * M.value[0][2] + value[2][1] * M.value[1][2] + value[2][2] * M.value[2][2]
            };
            return Matrix3(M_v);
        }
    };


    typedef enum {
        NULLPTR = 0,
        FIRECONTROL_RADAR,			//火控雷达
        EARLYWARNING_RADAR,			//预警雷达
        ANTI_SUBMARINE_SYSTEM,		//反潜系统
        RADARWARNING_RECEIVER,		//雷达告警系统
        MTI, FTI, ISAR				//无人机SAR成像雷达（动目标探测、固定目标探测、舰船成像）
    } RadarType;

    struct RadarBasicInfo {
        Vector3 ownP;
        Vector3 tarP;
        Vector3 ownV;
        Vector3 tarV;
        RadarType type;
    };

    struct TemporaryDetectSwitch {
        int RD_Mode;//模型的运行模式 Mode为0表示依据GranularityLevel的值决定粒度，Mode为1表示依据各项开关控制量决定模型的粒度
        bool RD_FlagMeasurement;//测距测速模型
        bool RD_FlagNoise;//接收机噪声模型
        bool RD_FlagRandomization;//随机探测模型
        bool RD_FlagSurfaceClutter;//面杂波模型
        bool RD_FlagTargetRCS;//目标散射模型
        bool RD_FlagTerrain;//地形模型
        bool RD_FlagVolumeClutter;//体杂波模型

        bool RD_HasJam;//[独立]是否考虑干扰
        bool RD_HasClutter;//[独立]是否考虑杂波
        int RD_JammerNum;//[独立]干扰机数量
    };

    struct RADARSTATE {	//雷达状态参数
        //位置与速度
        Vector3 rPos, rVel;
        double Height;		//雷达高度
        double fre_center; //雷达中心频率
        double fre_band; //雷达带宽
    };

    struct TARGETSTATE {	//目标状态参数
        int id;
        //几何参数
        Vector3 tPos, tVel;
        double RCS;//目标RCS
        short surfaceType;   //目标是处于海面还是地面。计算杂波使用
        bool detected; // 是否被探测到
        TARGETSTATE() :detected(false), surfaceType(0), RCS(0) {};
    };

    struct RadarParams {
        double ScanAzRange;//天线扫描方位角度范围
        double ScanElRangeUl;//天线扫描俯仰角上限
        double ScanElRangeLl;//天线扫描俯仰角下限
        double ThetaAZ05;//天线方位半功率角
        double ThetaEL05;//天线俯仰半功率角
        double ScanPeriod;//天线扫描周期

        double PRF;//脉冲重复频率
        double Tpulse;//有效脉冲宽度
        double Tgate;//波门宽度
        int NFilters;//每波门多普勒滤波器数目
        double Frequency;//雷达载频Hz
        double Pt; //峰值功率
        double fre_center;
        double fre_band;
    };

    //------------------------------------------------------------------------------
    // Class: Waypoint
    //------------------------------------------------------------------------------
    class Waypoint {
    public:
        double radLat{};
        double radLon{};
        double meterAlt{};
        double mpsSpeed{};
        double radHeading{};
        Waypoint() {};
        Waypoint(double lon, double lat, double alt, double speed,
            double heading) : radLat(lat* DEG2RAD), radLon(lon* DEG2RAD), meterAlt(alt),
            mpsSpeed(speed), radHeading(heading* DEG2RAD) {};
    };

}