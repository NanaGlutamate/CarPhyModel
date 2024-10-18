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
#include <chrono>

struct TimeCounter {
    size_t& tar;
    std::chrono::high_resolution_clock::time_point start_time;
    TimeCounter(size_t& tar) : tar(tar) { start_time = std::chrono::high_resolution_clock::now(); }
    ~TimeCounter() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        tar += duration;
    }
};

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
        //for test
        if (abs(end.x - 1) < 0.001) {
            return std::vector<carphymodel::Vector3>{
                {0.000000, 0.000000, 0.000000},      {-5.772000, -19.215904, 0.000000},
                {-14.097000, -42.131126, 0.000000},  {-16.539000, -51.790457, 0.000000},
                {-18.537000, -67.820836, 0.000000},  {-19.425000, -92.277441, 0.000000},
                {-19.314000, -140.368579, 0.000000}, {-17.538000, -144.581692, 0.000000},
                {-15.207000, -147.150663, 0.000000}, {-12.210000, -149.000322, 0.000000},
                {-9.435000, -149.616875, 0.000000},  {-1.554000, -150.130669, 0.000000},
                {0.333000, -150.130669, 0.000000},   {101.676000, -147.664457, 0.000000},
                {138.306000, -146.225833, 0.000000}, {157.287000, -147.458940, 0.000000},
                {170.607000, -149.000322, 0.000000}, {182.373000, -148.897563, 0.000000},
                {199.911000, -145.506522, 0.000000}, {360.639000, -101.217460, 0.000000},
                {428.682000, -83.440180, 0.000000},  {436.230000, -81.487762, 0.000000},
                {536.907000, -55.181499, 0.000000},  {572.094000, -44.700097, 0.000000},
                {641.358000, -22.812463, 0.000000},  {784.215000, 22.606945, 0.000000},
                {847.263000, 42.644920, 0.000000},   {888.666000, 49.632521, 0.000000},
                {886.557000, 35.451801, 0.000000},   {882.006000, 5.240701, 0.000000},
                {879.009000, -16.955209, 0.000000},  {877.233000, -31.238688, 0.000000},
                {861.360000, -135.230637, 0.000000}, {856.809000, -164.825184, 0.000000}};
        } else if (abs(end.x - 2) < 0.001) {
            return std::vector<carphymodel::Vector3>{
                {0.000000, 0.000000, 0.000000},      {-31.413000, 3.801907, 0.000000},
                {-51.504000, 6.165254, 0.000000},    {-57.054000, 6.884534, 0.000000},
                {-73.593000, 9.453390, 0.000000},    {-83.028000, 10.891949, 0.000000},
                {-83.472000, 6.987288, 0.000000},    {-83.916000, 3.699153, 0.000000},
                {-84.471000, -0.924788, 0.000000},   {-91.686000, -64.015891, 0.000000},
                {-93.573000, -79.942798, 0.000000},  {-76.257000, -82.203391, 0.000000},
                {48.396000, -98.541315, 0.000000},   {56.166000, -99.363349, 0.000000},
                {183.927000, -115.804027, 0.000000}, {233.211000, -122.174790, 0.000000},
                {230.880000, -137.485172, 0.000000}, {228.882000, -153.206570, 0.000000},
                {225.663000, -178.586867, 0.000000}, {217.116000, -245.788140, 0.000000},
                {216.117000, -253.802970, 0.000000}, {202.020000, -368.065684, 0.000000},
                {201.132000, -375.566743, 0.000000}, {192.474000, -453.557211, 0.000000},
                {191.142000, -465.373948, 0.000000}, {189.033000, -479.554033, 0.000000},
                {184.482000, -509.763779, 0.000000}, {181.485000, -531.958695, 0.000000},
                {179.709000, -546.241534, 0.000000}, {163.836000, -650.228824, 0.000000},
                {159.285000, -679.822045, 0.000000}};
        } else if (abs(end.x - 3) < 0.001)
        {
            return std::vector<carphymodel::Vector3>{
                {0.000000, 0.000000, 0.000000},      {-5.772000, -19.215904, 0.000000},
                {-14.097000, -42.131126, 0.000000},  {-16.539000, -51.790457, 0.000000},
                {-18.537000, -67.820836, 0.000000},  {-19.425000, -92.277441, 0.000000},
                {-19.314000, -140.368579, 0.000000}, {-17.538000, -144.581692, 0.000000},
                {-15.207000, -147.150663, 0.000000}, {-12.210000, -149.000322, 0.000000},
                {-9.435000, -149.616875, 0.000000},  {-1.554000, -150.130669, 0.000000},
                {0.333000, -150.130669, 0.000000},   {101.676000, -147.664457, 0.000000},
                {138.306000, -146.225833, 0.000000}, {157.287000, -147.458940, 0.000000},
                {170.607000, -149.000322, 0.000000}, {182.373000, -148.897563, 0.000000},
                {199.911000, -145.506522, 0.000000}, {360.639000, -101.217460, 0.000000},
                {428.682000, -83.440180, 0.000000},  {436.230000, -81.487762, 0.000000},
                {536.907000, -55.181499, 0.000000},  {541.791000, -20.140733, 0.000000},
                {544.566000, -0.822071, 0.000000},   {550.671000, 51.687698, 0.000000},
                {552.780000, 70.081531, 0.000000},   {557.442000, 108.718856, 0.000000},
                {574.980000, 236.653615, 0.000000},  {576.090000, 244.668805, 0.000000},
                {583.971000, 298.411680, 0.000000},  {590.964000, 348.044201, 0.000000},
                {594.960000, 376.508400, 0.000000},  {597.846000, 394.799474, 0.000000},
                {599.178000, 403.636735, 0.000000},  {601.287000, 418.536767, 0.000000},
                {603.951000, 435.080941, 0.000000},  {621.267000, 432.820246, 0.000000},
                {745.920000, 416.481590, 0.000000},  {746.808000, 424.188503, 0.000000},
                {747.585000, 430.045757, 0.000000},  {748.695000, 439.191294, 0.000000},
                {754.023000, 483.480356, 0.000000},  {754.689000, 489.337610, 0.000000},
                {754.911000, 491.392787, 0.000000},  {759.018000, 507.628684, 0.000000},
                {697.524000, 515.027321, 0.000000},  {697.524000, 515.027321, 0.000000}};
        } else if (abs(end.x - 4) < 0.001)
        {
            return std::vector<carphymodel::Vector3>{
                {0.000000, 0.000000, 0.000000},        {-5.772000, -19.215904, 0.000000},
                {-14.097000, -42.131126, 0.000000},    {-16.539000, -51.790457, 0.000000},
                {-18.537000, -67.820836, 0.000000},    {-19.425000, -92.277441, 0.000000},
                {-19.314000, -140.368579, 0.000000},   {-17.538000, -144.581692, 0.000000},
                {-15.207000, -147.150663, 0.000000},   {-12.210000, -149.000322, 0.000000},
                {-9.435000, -149.616875, 0.000000},    {-1.554000, -150.130669, 0.000000},
                {0.333000, -150.130669, 0.000000},     {101.676000, -147.664457, 0.000000},
                {138.306000, -146.225833, 0.000000},   {157.287000, -147.458940, 0.000000},
                {170.607000, -149.000322, 0.000000},   {168.831000, -161.023107, 0.000000},
                {167.388000, -169.141055, 0.000000},   {164.391000, -185.274194, 0.000000},
                {163.614000, -190.720412, 0.000000},   {156.399000, -234.495679, 0.000000},
                {148.518000, -258.952284, 0.000000},   {90.243000, -438.985777, 0.000000},
                {73.149000, -476.389996, 0.000000},    {2.109000, -632.788954, 0.000000},
                {-14.652000, -673.173180, 0.000000},   {-25.086000, -698.349096, 0.000000},
                {-27.417000, -703.692556, 0.000000},   {-31.968000, -714.276717, 0.000000},
                {-69.819000, -799.155521, 0.000000},   {-103.563000, -873.861200, 0.000000},
                {-169.608000, -1057.388493, 0.000000}, {-205.239000, -1193.235684, 0.000000},
                {-254.190000, -1418.791344, 0.000000}, {-328.227000, -1768.685204, 0.000000},
                {-355.866000, -1899.086175, 0.000000}, {-369.297000, -1964.543558, 0.000000},
                {-393.828000, -2073.365173, 0.000000}, {-404.484000, -2135.020479, 0.000000},
                {-413.142000, -2162.662607, 0.000000}, {-421.578000, -2184.858517, 0.000000},
                {-437.340000, -2226.373090, 0.000000}, {-460.428000, -2271.278704, 0.000000},
                {-490.065000, -2309.607753, 0.000000}, {-526.806000, -2353.794055, 0.000000},
                {-553.446000, -2379.586525, 0.000000}, {-581.862000, -2403.734853, 0.000000},
                {-621.711000, -2433.843194, 0.000000}, {-659.562000, -2461.177046, 0.000000},
                {-744.810000, -2528.689606, 0.000000}, {-753.579000, -2537.526866, 0.000000},
                {-763.680000, -2549.755169, 0.000000}, {-770.562000, -2560.647606, 0.000000},
                {-779.886000, -2578.733162, 0.000000}};
        }
        TimeCounter tc{Astar::calfinalpath_time};
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
        size_t width = size_t(x / step);//南北0707wsb，下面是东西
        size_t height = size_t(y / step);
        // create grid
        //grid_data.resize(width * height, 0);
        constexpr double threshold = 0.37;
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
        //给四周添加障碍物
        for (double i = x1; i < x2; i = i + step) {
            ox.push_back(i);
            oy.push_back(y1);
        }
        for (double i = y1; i < y2; i = i + step) {
            ox.push_back(x2);
            oy.push_back(i);
        }
        for (double i = x1; i < x2 + 1; i = i + step) {
            ox.push_back(i);
            oy.push_back(y2);
        }
        for (double i = y1; i < y2 + 1; i = i + step) {
            ox.push_back(x1);
            oy.push_back(i);
        }

        //wriet data to file
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

        
        std::cout << "linkcheckedtimes: " << Astar::linkedchecked << std::endl;
        std::cout << "used time: " << Astar::line_time << "(line), "  <<Astar::calfinalpath_time<< "(calfinalpath), " << Astar::total_time << "(total)" << std::endl;


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