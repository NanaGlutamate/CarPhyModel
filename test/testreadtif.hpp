#include "../src/model/environment/wsf.hpp"
#include <fstream>


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
    wsfplugin::WSFEnvironment wangshuaibo;
    wangshuaibo.init("D:\\codeproject\\perlinnoise\\test1.tif", 109.90997314453125, 36.14501953125);
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

    std::vector<carphymodel::Vector3> pathroad = wangshuaibo.getRoute({-54900, 54900, 0}, {-54900, 54700, 0});
    std::ofstream outputFile("path_output_road.txt", std::ios::out);
    if (outputFile.is_open()) {
        for (const auto& point : pathroad) {
            outputFile << point.x << "," << point.y << "," << point.z << std::endl;
        }
        outputFile.close();
        std::cout << "Data written to 'path_output_road.txt' successfully." << std::endl;
    } else {
        std::cerr << "Unable to open the output file." << std::endl;
    }
}

} // namespace wsfplugin