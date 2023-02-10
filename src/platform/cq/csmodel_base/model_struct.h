#ifndef CS_MODEL_STRUCT_H
#define CS_MODEL_STRUCT_H

#include <vector>
#include <any>
#include <unordered_map>

using CSValueMap = std::unordered_map<std::string, std::any>;

// 实例状态
enum class CSInstanceState {
  IS_UNSPECIFIED = 0,
  IS_CREATED = 1,		// 实例被创建
  IS_INITIALIZED = 2,	// 实例完成初始化
  IS_RUNNING = 3,		// 实例运行中
  IS_STOPPED = 4,		// 实例停止运算但暂不需销毁
  IS_DESTROYED = 5,		// 实例等候销毁
  IS_ERROR = 6			// 实例内部运算发生错误
};

// 参数基类
class CSParamBase {
 public:
  // 转换为std::unordered_map
  virtual CSValueMap ToValueMap() = 0;
  // 转换自std::unordered_map
  virtual void FromValueMap(CSValueMap&) = 0;

 public:
  // 结构体数组转换为std::any
  template <class T>
  static std::any ConvertArrayToAny(std::vector<T>& raw_array_) {
    std::vector<std::any> results;
    for (auto& vv : raw_array_) {
      results.push_back(vv.ToValueMap());
    }
    return results;
  }

  // std::any转换为结构体数组
  template <class T>
  static std::vector<T> ConvertAnyToArray(const std::any& params) {
    auto va = std::any_cast<std::vector<std::any>>(params);
    std::vector<T> v;
    for (auto& p : va) {
      auto pp = std::any_cast<CSValueMap>(p);
      T tv_;
      tv_.FromValueMap(pp);
      v.push_back(tv_);
    }
    return v;
  }
};

// 二维点
class CSVector2D final : public CSParamBase {
 public:
  //x :经度 y:维度
  CSVector2D() : x(0.0), y(0.0) {}
  CSVector2D(double x_, double y_) : x(x_), y(y_) {}
  CSVector2D(CSValueMap& v) {
    FromValueMap(v);
  }

  virtual CSValueMap ToValueMap() override {
    return std::move(CSValueMap{
      { "x", x },
      { "y", y }
    });
  }
  virtual void FromValueMap(CSValueMap& v) override {
    for (auto &it : v) {
      if (it.first == "x")
        x = std::any_cast<double>(it.second);
      if (it.first == "y")
        y = std::any_cast<double>(it.second);
    }
  }

 public:
  double GetX() {
    return x;
  }

  double GetY() {
    return y;
  }

  void SetX(double x_) {
    x = x_;
  }

  void SetY(double y_) {
    y = y_;
  }

 private:
  double x, y;
};

// 三维点
class CSVector3D final : public CSParamBase {
 public:
  //x :经度 y:维度 z:高度
  CSVector3D() : x(0.0), y(0.0), z(0.0) {}
  CSVector3D(double x_, double y_, double z_)
    : x(x_), y(y_), z(z_) {}
  CSVector3D(CSValueMap& v) {
    FromValueMap(v);
  }

  virtual CSValueMap ToValueMap() override {
    return std::move(CSValueMap{
      { "x", x },
      { "y", y },
      { "z", z }
    });
  }
  virtual void FromValueMap(CSValueMap& v) override {
    for (auto &it : v) {
      if (it.first == "x")
        x = std::any_cast<double>(it.second);
      if (it.first == "y")
        y = std::any_cast<double>(it.second);
      if (it.first == "z")
        z = std::any_cast<double>(it.second);
    }
  }

 public:
  double GetX() {
    return x;
  }

  double GetY() {
    return y;
  }

  double GetZ() {
    return z;
  }

  void SetX(double x_) {
    x = x_;
  }

  void SetY(double y_) {
    y = y_;
  }

  void SetZ(double z_) {
    z = z_;
  }

 private:
  double x, y, z;
};

// 四维点
class CSVector4D final : public CSParamBase {
 public:
  //x :经度 y:维度 z:高度 w:速度
  CSVector4D() : x(0.0), y(0.0), z(0.0), w(0.0) {}
  CSVector4D(double x_, double y_, double z_, double w_)
    : x(x_), y(y_), z(z_), w(w_) {}
  CSVector4D(CSValueMap& v) {
    FromValueMap(v);
  }

  virtual CSValueMap ToValueMap() override {
    return std::move(CSValueMap{
      { "x", x },
      { "y", y },
      { "z", z },
      { "w", w }
    });
  }
  virtual void FromValueMap(CSValueMap& v) override {
    for (auto &it : v) {
      if (it.first == "x")
        x = std::any_cast<double>(it.second);
      if (it.first == "y")
        y = std::any_cast<double>(it.second);
      if (it.first == "z")
        z = std::any_cast<double>(it.second);
      if (it.first == "w")
        w = std::any_cast<double>(it.second);
    }
  }

 public:
  double GetX() {
    return x;
  }

  double GetY() {
    return y;
  }

  double GetZ() {
    return z;
  }

  double GetW() {
    return w;
  }

  void SetX(double x_) {
    x = x_;
  }

  void SetY(double y_) {
    y = y_;
  }

  void SetZ(double z_) {
    z = z_;
  }

  void SetW(double w_) {
    w = w_;
  }

 private:
  double x, y, z, w;
};

// 地图点元素
class CSMapPoint final : public CSParamBase {
 public:
  CSMapPoint() = default;
  CSMapPoint(uint64_t id_, const CSVector4D& p)
    :id(id_), point(p) {}
  CSMapPoint(CSValueMap& v) {
    FromValueMap(v);
  }

  virtual CSValueMap ToValueMap() override {
    return std::move(CSValueMap{
      { "id", id },
      { "point", point.ToValueMap() }
    });
  }
  virtual void FromValueMap(CSValueMap& v) override {
    for (auto &it : v) {
      if (it.first == "id")
        id = std::any_cast<uint64_t>(it.second);
      if (it.first == "point") {
        auto t = std::any_cast<CSValueMap>(it.second);
        point.FromValueMap(t);
      }  
    }
  }

  uint64_t GetId() const {
    return id;
  }
  const CSVector4D& GetPoint() const {
    return point;
  }

 private:
  // 元素ID
  uint64_t id;
  // 点数据
  CSVector4D point;
};

// 地图航路元素
class CSMapPath final : public CSParamBase {
 public:
  CSMapPath() = default;
  CSMapPath(uint64_t id_, const std::vector<CSVector4D>& points_)
    :id(id_), points(points_) {}
  CSMapPath(CSValueMap& v) {
    FromValueMap(v);
  }

  virtual CSValueMap ToValueMap() override {
    return std::move(CSValueMap{
      { "id", id },
      { "points", CSParamBase::ConvertArrayToAny<CSVector4D>(points) }
    });
  }
  virtual void FromValueMap(CSValueMap& v) override {
    for (auto &it : v) {
      if (it.first == "id")
        id = std::any_cast<uint64_t>(it.second);
      if (it.first == "points") {
        points = CSParamBase::ConvertAnyToArray<CSVector4D>(it.second);
      }
    }
  }

  uint64_t GetId() const {
    return id;
  }
  const std::vector<CSVector4D>& GetPoints() const {
    return points;
  }

 private:
  // 元素ID
  uint64_t id;
  // 航路点数据
  std::vector<CSVector4D> points;
};

// 地图航路元素
class CSMapRegion final : public CSParamBase {
 public:
  CSMapRegion() = default;
  CSMapRegion(uint64_t id_, const std::vector<CSVector4D>& points_)
    :id(id_), points(points_) {}
  CSMapRegion(CSValueMap& v) {
    FromValueMap(v);
  }

  virtual CSValueMap ToValueMap() override {
    return std::move(CSValueMap{
      { "id", id },
      { "points", CSParamBase::ConvertArrayToAny<CSVector4D>(points) }
    });
  }
  virtual void FromValueMap(CSValueMap& v) override {
    for (auto &it : v) {
      if (it.first == "id")
        id = std::any_cast<uint64_t>(it.second);
      if (it.first == "points") {
        points = CSParamBase::ConvertAnyToArray<CSVector4D>(it.second);
      }
    }
  }

  uint64_t GetId() const {
    return id;
  }
  const std::vector<CSVector4D>& GetPoints() const {
    return points;
  }

 private:
  // 元素ID
  uint64_t id;
  // 区域边界点数据
  std::vector<CSVector4D> points;
};

#endif // !CS_MODEL_STRUCT_H
