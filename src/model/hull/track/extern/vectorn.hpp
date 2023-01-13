#pragma once

#include "vector3.h"

// #ifndef __VECTOR2_HPP__
// #define __VECTOR2_HPP__

// #include <array>
// #include <type_traits>
// #include "vector3.h"
// // #include <initializer_list>

// namespace externModel::track{

// template <typename E, int N>
// class VectorN {
// public:
//     std::array<E, N> data;

//     VectorN() = default;
//     VectorN(const std::array<E, N>& data): data(data){};
//     // VectorN(const std::initializer_list<E>& data): data(data){};

//     // template <typename... Args>
//     // VectorN(Arg...args): data({std::forward<std::enable_if<sizeof...(Args)==N, E>::type>(args),...}) {};
//     // VectorN():data() {};
//     // VectorN(const VectorN& other) : data(other.data) {};
//     // VectorN(VectorN&& other) : data(other.data) {};
    
//     VectorN& operator=(const VectorN& other) {
//         data = other.data;
//         return *this;
//     };
//     VectorN operator=(const VectorN& other) const{
//         data = other.data;
//         return data;
//     };
//     VectorN& operator+=(const VectorN& other) {
//         for (int i = 0; i < N; i++) {
//             data[i] += other.data[i];
//         }
//         return *this;
//     };
//     VectorN& operator-=(const VectorN& other) {
//         for (int i = 0; i < N; i++) {
//             data[i] -= other.data[i];
//         }
//         return *this;
//     };
//     VectorN& operator*=(const E& other) {
//         for (int i = 0; i < N; i++) {
//             data[i] *= other;
//         }
//         return *this;
//     };
//     VectorN& operator/=(const E& other) {
//         for (int i = 0; i < N; i++) {
//             data[i] /= other;
//         }
//         return *this;
//     };
//     VectorN operator+(const VectorN& other) const {
//         VectorN result = *this;
//         result += other;
//         return result;
//     };
//     VectorN operator-(const VectorN& other) const {
//         VectorN result = *this;
//         result -= other;
//         return result;
//     };
//     VectorN operator*(const E& other) const {
//         VectorN result = *this;
//         result *= other;
//         return result;
//     };
//     VectorN operator/(const E& other) const {
//         VectorN result = *this;
//         result /= other;
//         return result;
//     };
//     E operator[](int i) const{
//         return data[i];
//     };
//     E& operator[](int i) {
//         return data[i];
//     }
//     E dot(const VectorN& other) const {
//         E result = 0;
//         for (int i = 0; i < N; i++) {
//             result += data[i] * other[i];
//         }
//         return result;
//     };
//     E norm() const {
//         return sqrt(dot(*this));
//     };
// };

// template<>
// struct VectorN<double, 3> : public Vector33{};

// }

// #endif