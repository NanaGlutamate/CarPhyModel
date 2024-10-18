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
    //������txt�ļ���D:\\codeproject\\CarPhyModel\\bin\\interception_log.txt
    ofstream outfile("D:\\codeproject\\CarPhyModel\\bin\\interception_log.txt");
    outfile.close();
    if (testmode == 1)//�����������----------------------------------����ʱע�͵�cardamage.cpp�ļ��е�clear
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
                     << "�Ƿ��׷���1Ϊ�׷���0Ϊ���׷��� " << tmp.isFirst << endl;*/
            }
            model.components.getSpecificSingleton<carphymodel::FireEventQueue>()->push_back(tmp);
            model.components.getSpecificSingleton<carphymodel::Coordinate>().value().position = {pos_x, 0, 0};
            model.components.getSpecificSingleton<carphymodel::Hull>().value().velocity = {v_x, 0, 0};
            model.tick(0.05);

            n += model.components.getSpecificSingleton<HitEventQueue>().value().size();
        }
        std::cout << "firsthit rate: " << n / x << endl;
    } else if (testmode == 2) {
        // ���Ե�ҩ��---------------------------------����ʱ��������Ϊ��ƫ��
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
        // ��ȡһ��txt�ļ���ͳ����ߡ�success���͡�fail���Ĵ�������ɹ��ʣ�������������
        // ���������������ظ���-----------------------------

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
                     << "�Ƿ��׷���1Ϊ�׷���0Ϊ���׷��� " << tmp.isFirst << endl;*/
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
    } else if (testmode == 4) // ������硢�����ٶȣ����ʱ�����Ǹ�Ϊ��ߵĵط����˶�ʱ�����Ǹ�Ϊ���¡�
    {
        carphymodel::CarModel model;
        buildBaseModel(model);
        double theta = PI/2;
        double theta1 = atan2(-180, 500);
        int i;
        for (i = 0; i < 200; ++i) {
            get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[1]) =
                carphymodel::EntityInfo{.position = {500 * cos(theta), 500 * sin(theta), -6},//�޸�zֵ���Ը������
                           .velocity = {0, 0, 0},
                           .baseInfo = {carphymodel::BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}};
            if (abs(get<1>(*model.components.getNormal<FireUnit>().begin()).presentDirection.yaw - theta) <= 0.001)
            {
                cout << "rotate finished! rotate speed: " << theta * 180 / PI / (i * 0.1) << "��/s" << endl;
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
    else if (testmode == 5) // �������-----------------------------------��̽������޸ĳ������
    {
        carphymodel::CarModel model;
        buildBaseModel(model);
        for (int i = 30; i < 200; ++i) {
            get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[1]) =
                carphymodel::EntityInfo{.position = {100.0 * i, 0, -1}, // �޸�zֵ���Ը������
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
        //��������--------------------------
        for (int i = 0; i < x; ++i) {
            carphymodel::CarModel model;
            buildBaseModel(model);
            get<1>((*(model.components.getSpecificSingleton<carphymodel::ScannedMemory>()))[1]) =
                carphymodel::EntityInfo{.position = {pos_x, 0, -1},
                           .velocity = {v_x, 0, 0},
                           .baseInfo = {carphymodel::BaseInfo::ENTITY_TYPE::CAR, 1, 1, DAMAGE_LEVEL::N}};
            auto& buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>().value();
            buffer.emplace(static_cast<carphymodel::command::COMMAND_TYPE>(10), any(tuple<double, double>(0, 1)));
            model.tick(0.05);//���������¼�
            // model.tick(0.1);

            carphymodel::FireEvent tmp;
            auto& buffer2 = model.components.getSpecificSingleton<carphymodel::EventBuffer>().value();
            if (auto it = buffer2.find(string("FireDataOut")); it != buffer2.end()) {
                tmp = any_cast<carphymodel::FireEvent>(it->second);
            }
            model.components.getSpecificSingleton<carphymodel::FireEventQueue>()->push_back(tmp);
            model.components.getSpecificSingleton<carphymodel::Coordinate>().value().position = {pos_x, 0, 0};
            model.components.getSpecificSingleton<carphymodel::Hull>().value().velocity = {v_x, 0, 0};
            model.tick(0.05);//�������
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
    //        // �ж�info�е�tuple�е�double�����Ƿ�==0
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