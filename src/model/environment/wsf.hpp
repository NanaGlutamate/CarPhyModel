#pragma once



#include <string>
#include <vector>
#include <queue>
#include <set>
#include <cmath>

#include <gdal.h>
#include <gdal_priv.h>
#include "../environment.h"
#include "../routeplan/Astar.h"

namespace wsfplugin {

struct WSFEnvironment : public carphymodel::Environment {
    // Dem data;
    double longitudeO;
    double latitudeO;
    GDALDataset* dataset;
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

        static struct CallOnce {
            CallOnce() { GDALAllRegister(); }
        } init;
        dataset = (GDALDataset*)GDALOpen(filePath.c_str(), GA_ReadOnly);
        if (dataset == nullptr) {
            return false;
        }

        dataset->GetGeoTransform(transform);

        bandCount = dataset->GetRasterCount();
        dataWidth = dataset->GetRasterXSize();
        dataHeight = dataset->GetRasterYSize();
        pData = std::make_shared<std::vector<double>>(dataWidth * dataHeight);
        dataset->RasterIO(GF_Read, 0, 0, dataWidth, dataHeight, pData.get()->data(), dataWidth, dataHeight, GDT_Float64,
                          bandCount, 0, 0, 0, 0);
        return true;
    }

    inline std::pair<double, double> pixelToGeo(const std::pair<double, double>& pixelCoord) const {
        // pixelCoord: (pixel_x, pixel_y)/(col, row)
        // return: geo_x, geo_y
        return {transform[0] + pixelCoord.first * transform[1] + pixelCoord.second * transform[2],
                transform[3] + pixelCoord.first * transform[4] + pixelCoord.second * transform[5]};
    }

    inline std::pair<double, double> GeoToPixel(const std::pair<double, double>& geoCoord) const {
        // geoCoord:geo_x, geo_y
        // return: (double)(pixel_x, pixel_y)/(col, row)
        double invDet = 1 / (transform[1] * transform[5] - transform[2] * transform[4]);

        return {(transform[5] * (geoCoord.first - transform[0]) - transform[2] * (geoCoord.second - transform[3])) *
                    invDet,

                (-transform[4] * (geoCoord.first - transform[0]) + transform[1] * (geoCoord.second - transform[3])) *
                    invDet};
    }

    virtual double getAltitude(const carphymodel::Vector3& pos) const override {
        auto [latitude, longitude] = positionToLonlat(pos);

        auto curr_pos = std::make_pair(longitude, latitude);
        //std::cout << "curr_pos:" << curr_pos.first << " " << curr_pos.second << std::endl;
        auto pixelCoord = GeoToPixel(curr_pos);
        int x1 = (int)pixelCoord.first;
        int y1 = (int)pixelCoord.second;
        int x2 = x1 + 1;
        int y2 = y1 + 1;

        double dx = pixelCoord.first - x1;
        double dy = pixelCoord.second - y1;

        double w1 = (1.0 - dx) * (1.0 - dy);
        double w2 = dx * (1.0 - dy);
        double w3 = (1.0 - dx) * dy;
        double w4 = dx * dy;

        double elevation1, elevation2, elevation3, elevation4;
        // GDALRasterBand* band = dataset->GetRasterBand(1);
        // band->RasterIO(GF_Read, x1, y1, 1, 1, &elevation1, 1, 1, GDT_Float64, 0, 0);
        // band->RasterIO(GF_Read, x2, y1, 1, 1, &elevation2, 1, 1, GDT_Float64, 0, 0);
        // band->RasterIO(GF_Read, x1, y2, 1, 1, &elevation3, 1, 1, GDT_Float64, 0, 0);
        // band->RasterIO(GF_Read, x2, y2, 1, 1, &elevation4, 1, 1, GDT_Float64, 0, 0);
        if (y1 * dataWidth + x1 >= (*pData).size() || y1 * dataWidth + x2 >= (*pData).size() ||
            y2 * dataWidth + x1 >= (*pData).size() || y2 * dataWidth + x2 >= (*pData).size() ||
            y1 * dataWidth + x1 < 0 || y1 * dataWidth + x2 < 0 || y2 * dataWidth + x1 < 0 || y2 * dataWidth + x2 < 0)
            return -999;
        elevation1 = (*pData)[y1 * dataWidth + x1];
        elevation2 = (*pData)[y1 * dataWidth + x2];
        elevation3 = (*pData)[y2 * dataWidth + x1];
        elevation4 = (*pData)[y2 * dataWidth + x2];
        //std::cout << "elevation:" << elevation1 << " " << elevation2 << " " << elevation3 << " " << elevation4
                  //<< std::endl;
        //std::cout << "w:" << w1 << " " << w2 << " " << w3 << " " << w4 << std::endl;
        return static_cast<double>(elevation1 * w1 + elevation2 * w2 + elevation3 * w3 + elevation4 * w4);
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

    virtual std::vector<carphymodel::Vector3> getRoute(const carphymodel::Vector3& start, const carphymodel::Vector3& end) const override {
        auto mymin = [](auto x, auto y) { return x < y ? x : y; };
        auto mymax = [](auto x, auto y) { return x > y ? x : y; };
        // build grid
        //std::vector<char> grid_data;
        std::vector<double> ox;
        std::vector<double> oy;
        // calculate corner coordination of grid
        double x1 = mymin(start.x, end.x);
        double y1 = mymin(start.y, end.y);
        double x2 = mymax(start.x, end.x);
        double y2 = mymax(start.y, end.y);
        double x, y;
        // calculate width and legth based on input
        x = std::abs(x1 - x2) == 0 ? 100 : std::abs(x1 - x2);
        y = std::abs(y1 - y2) == 0 ? 100 : std::abs(y1 - y2);
        // apply margin to grid
        x1 -= x / 2;
        y1 -= y / 2;
        x2 += x / 2;
        y2 += y / 2;
        // recalculate grid size
        x = std::abs(x1 - x2);
        y = std::abs(y1 - y2);
        constexpr size_t MAX_WIDTH_AND_HEITGHT = 200;
        double step = mymax(10, mymax(x, y) / MAX_WIDTH_AND_HEITGHT);
        // calculate grid size
        size_t width = size_t(x / step);
        size_t height = size_t(y / step);
        // create grid
        //grid_data.resize(width * height, 0);
        constexpr double threshold = 0.15;
        // fill grid with moveable cells using slope
        for (double i = 0; i < width; i++) {
            for (double j = 0; j < height; j++) {
                auto pos = carphymodel::Vector3(x1 + i * step, y1 + j * step, 0);
                auto slope_x = getSlope(pos, carphymodel::Vector3(1, 0, 0));
                auto slope_y = getSlope(pos, carphymodel::Vector3(0, 1, 0));
                auto slope = std::sqrt(slope_x * slope_x + slope_y * slope_y);
                if (slope > threshold) {
                    ox.push_back(x1 + i * step);
                    oy.push_back(y1 + j * step);
                    //grid_data[i* width + j] = 1;
                }
            }
        }
        for (double i = x1; i < x2; i++) {
            ox.push_back(i);
            oy.push_back(y1);
        }
        for (double i = y1; i < y2; i++) {
            ox.push_back(x2);
            oy.push_back(i);
        }
        for (double i = x1; i < x2 + 1; i++) {
            ox.push_back(i);
            oy.push_back(y2);
        }
        for (double i = y1; i < y2 + 1; i++) {
            ox.push_back(x1);
            oy.push_back(i);
        }
        std::ofstream outputFile("path_output_obstacle.txt", std::ios::out);
        if (outputFile.is_open()) {
            for (size_t i = 0; i < ox.size(); i++) {
                outputFile << ox[i] << "," << oy[i] << std::endl;
            }
            outputFile.close();
            std::cout << "Data written to 'path_output_obstacle.txt' successfully." << std::endl;
        } else {
            std::cerr << "Unable to open the output file." << std::endl;
        }
        // find start and end cell
        std::vector<double> startpos{start.x, start.y}, goal{end.x, end.y};
        double grid_size = step;//2.0;//就是step
        double robot_radius = 5.0;//车的半径
        Astar astar(step, robot_radius);
        astar.setGo(goal);
        astar.setSt(startpos);
        astar.setOx(ox); // 设置障碍物
        astar.setOy(oy);
        astar.calObstacleMap(ox, oy);
        astar.getMotionModel();
        std::vector<carphymodel::Vector3> path = astar.planning(startpos, goal);

        // TODO: check AI-generated code
        // find path using A* algorithm
        /* std::priority_queue<std::pair<double, std::pair<size_t, size_t>>, std::vector<std::pair<double, std::pair<size_t, size_t>>>, std::greater<std::pair<double, std::pair<size_t, size_t>>>> open_list;
        std::set<std::pair<size_t, size_t>> close_list;*/
        //输出path到控制台
        /*for (auto p : path) {
            std::cout << "(" << p.x << "," << p.y << "," << p.z << ")" << std::endl;
        }*/
        return path;
    }
    virtual ~WSFEnvironment() {
        if (dataset) {
            GDALClose(dataset);
            dataset = nullptr;
        }
    }
};

}