#pragma once

#define wsf wsb
#define WSF WSB

#include <string>

#include <gdal.h>
#include <gdal_priv.h>
#include "../environment.h"

namespace wsfplugin {

class WSFEnvironment : public carphymodel::Environment {
    // Dem data;
    double longitudeO;
    double latitudeO;
    GDALDataset* dataset;
    double transform[6];

    struct Lonlat {
        double latitude, longitude;
    };
    Lonlat positionToLonlat(const carphymodel::Vector3& position) const {
        constexpr inline double rate = 111000.;
        return {.latitude = position.x / rate + latitudeO,
                .longitude = position.y / (rate * cos(carphymodel::DEG2RAD(latitudeO))) + longitudeO};
    }

  public:
    bool init(const std::string& filePath, double longitudeO, double latitudeO) {
        this->longitudeO = longitudeO;
        this->latitudeO = latitudeO;
        // read file
        static struct CallOnce {
            CallOnce() { GDALAllRegister(); }
        } init;
        GDALDataset* dataset = (GDALDataset*)GDALOpen(filePath.c_str(), GA_ReadOnly);
        if (dataset == nullptr) {
            return false;
        }
        dataset->GetGeoTransform(transform);
    }
    virtual double getAltitude(const carphymodel::Vector3& pos) const override {
        auto [latitude, longitude] = positionToLonlat(pos);

        int col = (int)((longitude - transform[0]) / transform[1]);
        int row = (int)((latitude - transform[3]) / transform[5]);
        // TODO: bilinear interpolation

        double elevation;
        GDALRasterBand* band = dataset->GetRasterBand(1); // 假设DEM只有一个波段
        band->RasterIO(GF_Read, col, row, 1, 1, &elevation, 1, 1, GDT_Float64, 0, 0);
        return elevation;
    }
    virtual double getSlope(const carphymodel::Vector3& pos, const carphymodel::Vector3& dir) const override {
        constexpr double length = 0.1;
        auto a0 = getAltitude(pos);
        auto a1 = getAltitude(pos + length * dir);
        return (a1 - a0) / length;
    }
    virtual bool getIntervisibility(const carphymodel::Vector3& pos1, const carphymodel::Vector3& pos2) const override {
        constexpr double step = 10.;
        auto a1 = getAltitude(pos1);
        auto a2 = getAltitude(pos2);
        auto intercept = [](double v1, double v2, double rate) { return rate * v1 + (1.0 - rate) * v2; };

        auto dir = pos2 - pos1;
        auto distance = (pos2 - pos1).norm();
        size_t cnt = size_t(distance / step);
        for (size_t i = 1; i < cnt; ++i) {
            double rate = double(i)/cnt;
            if (getAltitude(pos1 + rate*dir) > intercept(a1, a2, rate)) {
                return false;
            }
        }
        return true;
    }

    virtual ~WSFEnvironment() {
        if (dataset) {
            GDALClose(dataset);
            dataset = nullptr;
        }
    }
};

} // namespace carphymodel