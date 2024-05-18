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
        constexpr double rate = 111000.;
        return {.latitude = position.x / rate + latitudeO,
                .longitude = position.y / (rate * cos(carphymodel::DEG2RAD(latitudeO))) + longitudeO};
    }

  public:
    bool init(const std::string& filePath, double longitudeO, double latitudeO) {
        this->longitudeO = longitudeO;
        this->latitudeO = latitudeO;
        static struct CallOnce {
            CallOnce() { GDALAllRegister(); }
        } init;
        dataset = (GDALDataset*)GDALOpen(filePath.c_str(), GA_ReadOnly);
        if (dataset == nullptr) {
            return false;
        }
        dataset->GetGeoTransform(transform);
    }
    virtual double getAltitude(const carphymodel::Vector3& pos) const override {
        auto [latitude, longitude] = positionToLonlat(pos);

        double x = (longitude - transform[0]) / transform[1];
        double y = (latitude - transform[3]) / transform[5];

        int x1 = (int)x;
        int y1 = (int)y;

        int x2 = x1 + 1;
        int y2 = y1 + 1;

        double dx = x - x1;
        double dy = y - y1;

        double w1 = (1.0 - dx) * (1.0 - dy);
        double w2 = dx * (1.0 - dy);
        double w3 = (1.0 - dx) * dy;
        double w4 = dx * dy;

        double elevation1, elevation2, elevation3, elevation4;

        GDALRasterBand* band = dataset->GetRasterBand(1); // ����DEMֻ��һ������
        band->RasterIO(GF_Read, x1, y1, 1, 1, &elevation1, 1, 1, GDT_Float64, 0, 0);
        band->RasterIO(GF_Read, x2, y1, 1, 1, &elevation2, 1, 1, GDT_Float64, 0, 0);
        band->RasterIO(GF_Read, x1, y2, 1, 1, &elevation3, 1, 1, GDT_Float64, 0, 0);
        band->RasterIO(GF_Read, x2, y2, 1, 1, &elevation4, 1, 1, GDT_Float64, 0, 0);

        return elevation1 * w1 + elevation2 * w2 + elevation3 * w3 + elevation4 * w4;
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