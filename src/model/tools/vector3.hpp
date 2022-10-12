#ifndef __SRC_MODEL_VECTOR3_H__
#define __SRC_MODEL_VECTOR3_H__

#include <cmath>
#include <iostream>

//三维向量
struct Vector3{
    using element=double;
	element x, y, z;
	Vector3()=default;
	Vector3(element x, element y, element z):x(x), y(y), z(z) {};
	Vector3(const Vector3& v):x(v.x), y(v.y), z(v.z) {};
	void operator=(const Vector3& v){
        x = v.x;
        y = v.y;
        z = v.z;
    };
	Vector3 operator+(const Vector3& v) const{
        return Vector3(x + v.x, y + v.y, z + v.z);
    };
	Vector3 operator-(const Vector3& v) const{
        return Vector3(x - v.x, y - v.y, z - v.z);
    };
	Vector3 operator*(element f) const{
        return Vector3(x * f, y * f, z * f);
    };
    friend Vector3 operator*(element f, const Vector3& v){
        return Vector3(v.x * f, v.y * f, v.z * f);
    }
	Vector3 operator/(element f) const{
        return Vector3(x / f, y / f, z / f);
    };
	void operator+=(const Vector3& v){
        x += v.x;
        y += v.y;
        z += v.z;
    };
	void operator-=(const Vector3& v){
        x -= v.x;
        y -= v.y;
        z -= v.z;
    };
	void operator*=(element f){
        x *= f;
        y *= f;
        z *= f;
    };
	void operator/=(element f){
        x /= f;
        y /= f;
        z /= f;
    };
    inline element dot(const Vector3& v) const{
        return x * v.x + y * v.y + z * v.z;
    };
    inline Vector3 out(const Vector3& v) const{
        return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    };
    inline element norm() const{
        using std::sqrt;
        return sqrt(x * x + y * y + z * z);
    };

    //! act as a rotation quaternion (x, y, z, w=sqrt(1-x^2-y^2-z^2))
    //! @param p original position
    //! @return position after rotation
    Vector3 rotate(const Vector3& p) const;

    element operator[](size_t index) const{
        return (&x)[index];
    };
    element& operator[](size_t index){
        return (&x)[index];
    };
};

//四元数
struct Quaternion{
    using element=double;
    element x, y, z, w;
    Quaternion(){};
    Quaternion(element x, element y, element z, element w):x(x), y(y), z(z), w(w) {};
    Quaternion(const Quaternion& q):x(q.x), y(q.y), z(q.z), w(q.w) {};
    Quaternion(element yaw, element pitch, element roll){
        using std::sin;
        using std::cos;
        element cy = cos(yaw * 0.5);
        element sy = sin(yaw * 0.5);
        element cp = cos(pitch * 0.5);
        element sp = sin(pitch * 0.5);
        element cr = cos(roll * 0.5);
        element sr = sin(roll * 0.5);
        x = sr * cp * cy - cr * sp * sy;
        y = cr * sp * cy + sr * cp * sy;
        z = cr * cp * sy - sr * sp * cy;
        w = cr * cp * cy + sr * sp * sy;
    };

    //! 获得绕轴旋转的坐标变换对应的四元数
    //! @param axis 轴
    //! @param angle 旋转角
    explicit Quaternion(const Vector3& axis, element angle);

    //! 将坐标或压缩四元数扩展为四元数
    //! @param v 坐标或压缩四元数
    //! @param isCompressedQuaternion 是否为压缩四元数，如果是，则将w扩展为sqrt(1-x^2-y^2-z^2)，否则w为0
    explicit Quaternion(const Vector3& v, bool isCompressedQuaternion){
        x=v.x;
        y=v.y;
        z=v.z;
        w=isCompressedQuaternion?sqrt(1.-x*x-y*y-z*z):0.;
    };

    //! 将四元数转为欧拉角
    //! @return Vector3: x=roll, y=pitch, z=yaw
    inline Vector3 getEuler() const{
        using std::asin;
        using std::atan2;
        element roll = atan2(2 * (w * x + y * z), 1 - 2 * (x * x + y * y));
        element pitch = asin(2 * (w * y - z * x));
        element yaw = atan2(2 * (w * z + x * y), 1 - 2 * (y * y + z * z));
        return Vector3(roll, pitch, yaw);
    };
    void operator=(const Quaternion& q){
        x = q.x;
        y = q.y;
        z = q.z;
        w = q.w;
    };
    Quaternion operator+(const Quaternion& q) const{
        return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
    };
    Quaternion operator*(const Quaternion& q) const{
        return Quaternion(
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y + y * q.w + z * q.x - x * q.z,
            w * q.z + z * q.w + x * q.y - y * q.x,
            w * q.w - x * q.x - y * q.y - z * q.z
        );
    };
    Quaternion operator*(element f) const{
        return Quaternion(x * f, y * f, z * f, w * f);
    };
    Quaternion operator/(element f) const{
        return Quaternion(x / f, y / f, z / f, w / f);
    };
    Quaternion conjugate() const{
        return Quaternion(-x, -y, -z, w);
    };
    Quaternion inverse() const{
        return conjugate() / (x * x + y * y + z * z + w * w);
    };
    //用当前四元数旋转作为参数的向量四元数
    Quaternion rotate(const Quaternion& p) const{
        return (*this) * p * inverse();
    };
};

inline Quaternion::Quaternion(const Vector3& axis, element angle){
    using std::sin;
    using std::cos;
    Vector3 normlized = axis/axis.norm();
    element sin_a = sin(angle / 2);
    element cos_a = cos(angle / 2);
    x = normlized.x * sin_a;
    y = normlized.y * sin_a;
    z = normlized.z * sin_a;
    w = cos_a;
};

inline Vector3 Vector3::rotate(const Vector3& p) const{
    Quaternion q(*this, true);
    Quaternion pq(p, false);
    Quaternion r = q.rotate(pq);
    return Vector3(r.x, r.y, r.z);
};

#endif
