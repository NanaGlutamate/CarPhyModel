#include <iostream>
#include <tuple>

#include "../basetest.h"
//#include "../../platform/cq/CarPhyModel/CarPhyModel.h"

int main() {

    int testmode = 3;

    using namespace std;
    double n = 0;
    int x = 10000;
    double pos_x = 2600;
    double v_x = 0;
    //清空这个txt文件：D:\\codeproject\\CarPhyModel\\bin\\interception_log.txt
    ofstream outfile("D:\\codeproject\\CarPhyModel\\bin\\interception_log.txt");
    outfile.close();
    if (testmode == 1)//测试射击精度----------------------------------测试时注释掉cardamage.cpp文件中的clear
    {
        for (int i = 0; i < x; ++i) {
            carphymodel::CarModel model;
            buildBaseModel(model);
            get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[1]) =
                carphymodel::EntityInfo{.position = {pos_x, 0, 0},
                           .velocity = {v_x, 0, 0},
                           .baseInfo = {carphymodel::BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}};
            auto& buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>().value();
            buffer.emplace(static_cast<carphymodel::command::COMMAND_TYPE>(11), any(tuple<double, double>(0, 1)));

            model.tick(0.05);
            // model.tick(0.1);

            carphymodel::FireEvent tmp;
            auto& buffer2 = model.components.getSpecificSingleton<carphymodel::EventBuffer>().value();
            if (auto it = buffer2.find(string("FireDataOut")); it != buffer2.end()) {
                tmp = any_cast<carphymodel::FireEvent>(it->second);
                /*std::cout << format("carphymodel send fireEvent: {{weapon: {}, from: {}({}, {}, {}), to: ({}, {},
                   {})}}", tmp.weaponName, 0, tmp.position.x, tmp.position.y, tmp.position.z, tmp.target.x,
                               tmp.target.y, tmp.target.z) << endl
                     << "是否首发：1为首发，0为非首发： " << tmp.isFirst << endl;*/
            }
            model.components.getSpecificSingleton<carphymodel::FireEventQueue>()->push_back(tmp);
            model.components.getSpecificSingleton<carphymodel::Coordinate>().value().position = {pos_x, 0, 0};
            model.components.getSpecificSingleton<carphymodel::Hull>().value().velocity = {v_x, 0, 0};
            model.tick(0.05);

            n += model.components.getSpecificSingleton<HitEventQueue>().value().size();
        }
        std::cout << "firsthit rate: " << n / x << endl;
    } else if (testmode == 2) {
        // 测试弹药量---------------------------------测试时精度配置为无偏差
        carphymodel::CarModel model;
        buildBaseModel(model);
        for (int i = 0; i < 50; ++i) {
            get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[1]) =
                carphymodel::EntityInfo{.position = {pos_x, 0, 5},
                           .velocity = {0, 0, 0},
                           .baseInfo = {carphymodel::BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}};
            auto& buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>().value();
            buffer.emplace(static_cast<carphymodel::command::COMMAND_TYPE>(10), any(tuple<double, double>(0, 1)));
            model.tick(0.05);
            cout << "ammoremainnum: " << get<1>(*model.components.getNormal<FireUnit>().begin()).weapon.ammoRemain
                 << endl;
        }
    } else if (testmode == 3) {
        // 读取一个txt文件，统计里边“success”和“fail”的次数计算成功率，先清空里边内容
        // 测试主动防护拦截概率-----------------------------

        for (int i = 0; i < x; ++i) {
            carphymodel::CarModel model;
            buildBaseModel(model);
            get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[1]) =
                carphymodel::EntityInfo{.position = {pos_x, 0, 5},
                           .velocity = {v_x, 0, 0},
                           .baseInfo = {carphymodel::BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}};
            auto& buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>().value();
            buffer.emplace(static_cast<carphymodel::command::COMMAND_TYPE>(10), any(tuple<double, double>(0, 1)));

            model.tick(0.05);
            // model.tick(0.1);

            carphymodel::FireEvent tmp;
            auto& buffer2 = model.components.getSpecificSingleton<carphymodel::EventBuffer>().value();
            if (auto it = buffer2.find(string("FireDataOut")); it != buffer2.end()) {
                tmp = any_cast<carphymodel::FireEvent>(it->second);
                /*std::cout << format("carphymodel send fireEvent: {{weapon: {}, from: {}({}, {}, {}), to: ({}, {},
                   {})}}", tmp.weaponName, 0, tmp.position.x, tmp.position.y, tmp.position.z, tmp.target.x,
                               tmp.target.y, tmp.target.z) << endl
                     << "是否首发：1为首发，0为非首发： " << tmp.isFirst << endl;*/
            }
            model.components.getSpecificSingleton<carphymodel::FireEventQueue>()->push_back(tmp);
            model.components.getSpecificSingleton<carphymodel::Coordinate>().value().position = {pos_x, 0, 5};
            model.components.getSpecificSingleton<carphymodel::Hull>().value().velocity = {v_x, 0, 0};
            model.tick(0.05);

            n += model.components.getSpecificSingleton<HitEventQueue>().value().size();
        }

        ifstream infile("D:\\codeproject\\CarPhyModel\\bin\\interception_log.txt");
        string line;
        double success = 0;
        double fail = 0;
        while (getline(infile, line)) {
            if (line.find("success") != string::npos) {
                success++;
            } else if (line.find("fail") != string::npos) {
                fail++;
            }
        }
        infile.close();
        std::cout << "success rate: " << success / (success + fail) << endl;
    } else if (testmode == 4) // 测试射界、调炮速度，打击时俯仰角负为打高的地方。运动时俯仰角负为下坡。
    {
        carphymodel::CarModel model;
        buildBaseModel(model);
        double theta = PI/2;
        double theta1 = atan2(-180, 500);
        int i;
        for (i = 0; i < 200; ++i) {
            get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[1]) =
                carphymodel::EntityInfo{.position = {500 * cos(theta), 500 * sin(theta), -6},//修改z值测试俯仰射界
                           .velocity = {0, 0, 0},
                           .baseInfo = {carphymodel::BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}};
            if (abs(get<1>(*model.components.getNormal<FireUnit>().begin()).presentDirection.yaw - theta) <= 0.001)
            {
                cout << "rotate finished! rotate speed: " << theta * 180 / PI / (i * 0.1) << "°/s" << endl;
                break;
            }
            auto& buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>().value();
            buffer.emplace(static_cast<carphymodel::command::COMMAND_TYPE>(10), any(tuple<double, double>(0, 1)));
            model.tick(0.1);
            cout << "current weapon yaw: "
                 << get<1>(*model.components.getNormal<FireUnit>().begin()).presentDirection.yaw
                 << " current weapon pitch: "
                 << get<1>(*model.components.getNormal<FireUnit>().begin()).presentDirection.pitch << endl;
        }
        if (i == 200)
        {
            cout << "rotate range can't reach!!!" << endl;
        }
    }
    else if (testmode == 5) // 测试射程-----------------------------------把探测距离修改超过射程
    {
        carphymodel::CarModel model;
        buildBaseModel(model);
        for (int i = 30; i < 200; ++i) {
            get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[1]) =
                carphymodel::EntityInfo{.position = {100.0 * i, 0, -1}, // 修改z值测试俯仰射界
                           .velocity = {0, 0, 0},
                           .baseInfo = {carphymodel::BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}};
            auto& buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>().value();
            buffer.emplace(static_cast<carphymodel::command::COMMAND_TYPE>(10), any(tuple<double, double>(0, 1)));
            model.tick(0.05);
            carphymodel::FireEvent tmp;
            auto& buffer2 = model.components.getSpecificSingleton<carphymodel::EventBuffer>().value();
            if (auto it = buffer2.find(string("FireDataOut")); it != buffer2.end()) {
                tmp = any_cast<carphymodel::FireEvent>(it->second);
                cout << format("carphymodel send fireEvent: {{weapon: {}, from: {}({}, {}, {}), to: ({}, {}, {})}}",
                               tmp.weaponName, 0, tmp.position.x, tmp.position.y, tmp.position.z, tmp.target.x,
                               tmp.target.y, tmp.target.z) << endl;
            }
            else
            {
                cout << " out of range, can't fire reach!!!: " << 100.0 * i << "m"<< endl;
            }

        }
    }
    else if (testmode == 6)
    {
        //测试威力--------------------------
        for (int i = 0; i < x; ++i) {
            carphymodel::CarModel model;
            buildBaseModel(model);
            get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[1]) =
                carphymodel::EntityInfo{.position = {pos_x, 0, -1},
                           .velocity = {v_x, 0, 0},
                           .baseInfo = {carphymodel::BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}};
            auto& buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>().value();
            buffer.emplace(static_cast<carphymodel::command::COMMAND_TYPE>(10), any(tuple<double, double>(0, 1)));
            model.tick(0.05);//产生开火事件
            // model.tick(0.1);

            carphymodel::FireEvent tmp;
            auto& buffer2 = model.components.getSpecificSingleton<carphymodel::EventBuffer>().value();
            if (auto it = buffer2.find(string("FireDataOut")); it != buffer2.end()) {
                tmp = any_cast<carphymodel::FireEvent>(it->second);
            }
            model.components.getSpecificSingleton<carphymodel::FireEventQueue>()->push_back(tmp);
            model.components.getSpecificSingleton<carphymodel::Coordinate>().value().position = {pos_x, 0, 0};
            model.components.getSpecificSingleton<carphymodel::Hull>().value().velocity = {v_x, 0, 0};
            model.tick(0.05);//解算毁伤
        }
    }
    //else if (testmode == 7)
    //{
    //    carphymodel::CarModel model;
    //    buildBaseModel(model);
    //    get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[1]) =
    //        carphymodel::EntityInfo{.position = {pos_x, 0, -1},
    //                   .velocity = {v_x, 0, 0},
    //                   .baseInfo = {carphymodel::BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}};
    //    auto& buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>().value();
    //    buffer.emplace(static_cast<carphymodel::command::COMMAND_TYPE>(10), any(tuple<double, double>(0, 1)));
    //    model.tick(0.05); 
    //    auto& buffer1 = model.components.getSpecificSingleton<carphymodel::EventBuffer>();
    //    std::vector<std::any> scannedInfoOut;
    //    for (auto& info : model.components.getSpecificSingleton<carphymodel::ScannedMemory>().value()) {
    //        // 判断info中的tuple中的double参数是否==0
    //        if (std::get<0>(info.second) == 0.) {
    //            // scannedInfoOut.emplace_back(EntityInfo(std::get<1>(info.second)).ToValueMap());
    //        }
    //        scannedInfoOut.emplace_back(::EntityInfo(std::get<1>(info.second)).ToValueMap());
    //    }
    //    buffer1.emplace("scannedInfoOut", std::move(scannedInfoOut));

    //    carphymodel::FireEvent tmp;
    //    auto& buffer2 = model.components.getSpecificSingleton<carphymodel::EventBuffer>().value();
    //    if (auto it = buffer2.find(string("FireDataOut")); it != buffer2.end()) {
    //        tmp = any_cast<carphymodel::FireEvent>(it->second);
    //    }

    //}
    return 0;
}