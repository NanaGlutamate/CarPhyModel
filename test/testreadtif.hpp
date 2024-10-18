#include "../src/model/environment/wsf.hpp"
#include <fstream>

#include "../../../../LML/include/libmiddleware.h"


namespace wsfplugin {

constexpr inline double rate = 111000.;
carphymodel::Vector3 locationTrans(double baseLongitude, double baseLatitude, double longitude, double latitude, double altitude) {
    return {
        rate * (latitude - baseLatitude),
        rate * (longitude - baseLongitude) * cos(carphymodel::DEG2RAD(baseLatitude)),
        -altitude,
    };
}

void testmain() {

    char serveraddress[] = "10.134.114.97:50052"; // 创建一个字符数组
    char rabbitmqaddress[] = "amqp://guest:guest@10.134.114.97:5672"; // 创建一个字符数组
    InitMiddleware(serveraddress, nullptr, rabbitmqaddress);
    // GetRawData(34.46, 78.41, 34.47, 78.42, 14);
     GetRoutePoints_return params = GetRoutePoints(113.5439372, 22.2180642, 113.5425177, 22.2252363);
     printf("total points count: %d\n", params.r0);
     for (int i = 0; i < params.r0; i++) {
         printf("lon-lat: (%lf, %lf)\n", params.r1[i], params.r2[i]);
     }
     FreePositionsPointer(params.r1, params.r2);
    // UpdateObstacle(22.158472, 113.416793, "test obstacle");
    // UpdateCrater(78.45, 34.49, 5.33, 4.32);
    // double altitude = GetAltitude(78.45, 34.39);
    // printf("altitude at lon-lat: (%lf, %lf) is %lf\n", 78.45, 34.39, altitude);
    CloseMiddleware();


    /*wsfplugin::WSFEnvironment wangshuaibo;
    wangshuaibo.init("D:\\codeproject\\perlinnoise\\test1.tif", 109.90997314453125, 36.14501953125);*/
    /*double elevation = wangshuaibo.getAltitude({0, 0, 0});
    double elevation1 = wangshuaibo.getAltitude({0, 55000, 0});
    double elevation2 = wangshuaibo.getAltitude({-55000, 55000, 0});
    double elevation3 = wangshuaibo.getAltitude({-55000, 0, 0});
    printf("elevation: %f\n", elevation);
    printf("elevation1: %f\n", elevation1);
    printf("elevation2: %f\n", elevation2);
    printf("elevation3: %f\n", elevation3);
    auto ground = [&](double longitude, double latitude) { 
        auto p = locationTrans(wangshuaibo.longitudeO, wangshuaibo.latitudeO, longitude, latitude, 0);
        p.z = -wangshuaibo.getAltitude(p);
        return p;
    };
    bool djw = wangshuaibo.getIntervisibility(ground(110.4043, 35.7337), ground(110.4043, 35.7831));
    bool djw1 = wangshuaibo.getIntervisibility(ground(110.4043, 35.7337), ground(110.4043, 35.7800));
    bool djw2 = wangshuaibo.getIntervisibility(ground(110.4043, 35.7337), ground(110.4043, 35.73375));*/
    /*for (int i = 0; i < 5; i++)
        wangshuaibo.getRoute({-54900, 54000, 0}, {-49000, 54500, 0});*/

    //路径规划测试---------------------------
    /*std::vector<carphymodel::Vector3> pathroad = wangshuaibo.getRoute({-52500, 54000, 0}, {-50000, 54800, 0});

    std::ofstream outputFile("path_output_road.txt", std::ios::out);
    if (outputFile.is_open()) {
        for (const auto& point : pathroad) {
            outputFile << point.x << "," << point.y << "," << point.z << std::endl;
        }
        outputFile.close();
        std::cout << "Data written to 'path_output_road.txt' successfully." << std::endl;
    } else {
        std::cerr << "Unable to open the output file." << std::endl;
    }*/


    //平均速度测试---------------------------
    //using namespace std;
    //carphymodel::CarModel model;
    //buildBaseModel(model);
    //auto& damage = model.components.getSpecificSingleton<carphymodel::DamageModel>().value();
    //auto& buffer = model.components.getSpecificSingleton<carphymodel::CommandBuffer>().value();

    //auto env = std::make_unique<wsfplugin::WSFEnvironment>();
    //auto filePath = "D:/codeproject/simplecq-master/config/zjc_wsb_demo.tif";
    //env->init(filePath, 106.747741224040951, 36.92016763212966);
    //carphymodel::EnvironmentInfoAgent::changeEnvironmentSupplier(std::move(env));
    //model.tick(0.1);
    ////carphymodel::Vector3 pos_start = model.components.getSpecificSingleton<carphymodel::Coordinate>().value().position;
    //double tmpdis = 0;
    //int count = 8000;
    //double dt = 0.1;
    //for (int i = 0; i < count; ++i) {
    //    buffer.emplace(static_cast<carphymodel::command::COMMAND_TYPE>(2), any(tuple<double, double>(13.89, 0)));
    //    if (i > 3000)
    //    tmpdis += (model.components.getSpecificSingleton<Hull>().value().velocity * dt).norm();
    //    model.tick(dt);
    //    auto attitude = carphymodel::Quaternion::fromCompressedQuaternion(
    //                        model.components.getSpecificSingleton<carphymodel::Coordinate>().value().attitude)
    //                        .getEuler();
    //    cout << "roll: " << carphymodel::RAD2DEG(attitude.x) << " pitch: " << carphymodel::RAD2DEG(attitude.y)
    //         << " vel: " << model.components.getSpecificSingleton<carphymodel::Hull>().value().velocity.norm() << "m/s" <<  endl
    //        /*<< "longitude: " << */;
    //}
    ////auto pos_finish = model.components.getSpecificSingleton<carphymodel::Coordinate>().value().position;
    //cout << "average speed: " << tmpdis / ((count - 3000) * dt) * 3.6 << "km/h" << endl;

}

} // namespace wsfplugin