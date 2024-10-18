#pragma once
#pragma once

#include <cmath>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "../environment.h"
//#include "../routeplan/Astar.h"
#include <chrono>

namespace lmlplugin {

struct LMLEnvironment : public carphymodel::Environment {
    // Dem data;

    double longitudeO;
    double latitudeO;
    double transform[6];

    int bandCount;
    int dataWidth;
    int dataHeight;
    std::shared_ptr<std::vector<double>> pData;

    struct Lonlat {
        double latitude, longitude;
    };
    Lonlat positionToLonlat(const carphymodel::Vector3& position) const {
        constexpr double rate = 111000.;
        return {.latitude = position.x / rate + latitudeO,
                .longitude = position.y / (rate * cos(carphymodel::DEG2RAD(latitudeO))) + longitudeO};
    }

  public:
    bool init(const std::string& filePath, double longitudeO, double latitudeO) {
        this->longitudeO = longitudeO;
        this->latitudeO = latitudeO;

        return true;
    }

    virtual double getAltitude(const carphymodel::Vector3& pos) const override {
        auto [latitude, longitude] = positionToLonlat(pos);
        return 0;
    }

    virtual double getSlope(const carphymodel::Vector3& pos, const carphymodel::Vector3& dir) const override {
        constexpr double length = 1;
        auto a1 = getAltitude(pos);
        auto a2 = getAltitude(pos + length * dir);
        return (a2 - a1) / length;
    }

    virtual bool getIntervisibility(const carphymodel::Vector3& pos1, const carphymodel::Vector3& pos2) const override {
        constexpr double unit = 20.;
        auto a1 = -pos1.z;
        auto a2 = -pos2.z;
        auto dir = pos2 - pos1;
        auto distance = (pos2 - pos1).norm();
        size_t cnt = size_t(distance / unit);
        size_t step = 1;
        for (size_t i = 1; i < cnt; i += step) {
            double rate = double(i) / cnt;
            auto diff = getAltitude(pos1 + rate * dir) - (a1 + rate * (a2 - a1));
            if (diff >= 0.) {
                return false;
            }
        }
        return true;
    }

    virtual std::vector<carphymodel::Vector3> getRoute(const carphymodel::Vector3& start,
                                                       const carphymodel::Vector3& end) const override {
        // for test
        return path;
    }
    virtual ~LMLEnvironment() {

    }
};

} // namespace wsfplugin