#pragma once

#include "common_struct.h"

namespace comm
{

    typedef struct Atmosphere
    {
        double oxygen;
        double steam;
    };

    typedef struct relative_POS
    {
        double azimuth;
        double altitude;
        double d; // km

    }; //接收机相对于发射机的相对位置

    // Comm
    //
    class Comm
    {
    public:
        // 初始化
        virtual bool Init(const std::unordered_map<std::string, std::any> &value);
        // 单步运算 time 应推进的步长(ms)
        virtual bool Tick(double time);
        // 获取输入参数
        virtual bool SetInput(const std::unordered_map<std::string, std::any> &value);
        // 对外部输出模型参数
        virtual std::unordered_map<std::string, std::any> *GetOutput();
        std::unordered_map<std::string, std::any> Get_tranpos();
        double Get_tranp();
        double Get_trangt();
        double Get_trangtf();
        double Get_trangB();
        std::unordered_map<std::string, std::any> Get_tran();
        Comm_tran tranpair;
        double Atmos();
        double Gain(double f, int i, double A);
        void tran_Gain();
        void rec_Gain();
        // void N_Jammer();//干扰噪声功率dB
        void N_t(); //环境噪声功率dB
        void N();   //总的噪声功率dB
        void postion();
        void GAIN(); //总的天线增益dB
        void Ber();  //误码率

    private:
        CSValueMap params_;

        // 得到本设备的位置
        V3f Get_tranpos_;
        // 设备的位置信息
        V3f position_;
        // 载体的姿态信息
        V3f vehicle_;
        // 环境参数
        Env environment_;
        // 本设备的接收机参数
        Fixed_para rec_para_;
        // 发射机参数
        Fixed_para tran_para_;
        // 所要接收的设备的位置信息
        Comm_tran tran_;
        // 得到本设备的发射天线增益
        double Get_trangt_;
        // 计算得到误码率
        double BER_;
        // 得到本设备的带宽
        double Get_trangtB_;
        // 得到本设备的发射频率
        double Get_trangtf_;
        // 得到本设备的发射功率
        double Get_tranp_;
        Comm_tran out_;
        // 计算链路是否连通
        bool connect_;
        // 干扰功率
        double j_;
        double Gt;
        double Gr;
        double G;
        double NJ;        //干扰噪声
        double Nt;        //环境噪声
        double N0;        //总噪声
        relative_POS rel; //接收设备相对于发射设备的相对位置
        double ber;
        bool connect;
        //相对于地面坐标系的天线指向
        double azig;
        double altg;
    };

}