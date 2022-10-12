#include "basetest.h"

namespace test03 {
using namespace std;

void assertVector3Equal(const Vector3& v1, const Vector3& v2, unsigned int l){
    cout<<"line"<<l<<":"<<v1<<"=="<<v2<<endl;
    assert((v1-v2).norm()<1e-4);
    return;
}

void assertQuaternionEqual(const Quaternion& v1, const Quaternion& v2, unsigned int l){
    cout<<"line"<<l<<":"<<v1<<"=="<<v2<<endl;
    assert((fabs(v1.x-v2.x)+fabs(v1.y-v2.y)+fabs(v1.z-v2.z)+fabs(v1.w-v2.w))<1e-8);
    return;
}

const double PI = 3.1415926535897932384626433;

//vector3, quaternion and coordinate test
int test_main(){
    using namespace std;

    assertQuaternionEqual(Quaternion(Vector3(1., 0., 0.), 1.), Quaternion(Vector3(1., 0., 0.)*2, 1.), __LINE__);

    Quaternion q(Vector3{2.f, 0.f, 0.f}, PI/3.);
    Vector3 cq(q.x,q.y,q.z);

    Vector3 p(0.5,0.,1.);

    // 拷贝构造函数测试
    auto p1(p);
    assertVector3Equal(p1, p, __LINE__);

    // 压缩四元数旋转测试
    p1=cq.rotate(p);
    assertVector3Equal(p1, {0.5,-sqrt(3)/2,0.5}, __LINE__);
    p1=cq.rotate(p1);
    assertVector3Equal(p1, {0.5,-sqrt(3)/2,-0.5}, __LINE__);
    p1=cq.rotate(p1);
    assertVector3Equal(p1, {0.5,0.,-1.}, __LINE__);

    // 坐标系变换测试
    Quaternion q1(Vector3{1.f, 0.f, 0.f}, PI/4.);
    Coordinate c1{Vector3(0.0,0.0,1.0), {q1.x, q1.y, q1.z}};
    p1=c1.positionWorldToBody(Vector3(1.0,1.0,1.0));
    assertVector3Equal(p1, {1., sqrt(2)/2, -sqrt(2)/2}, __LINE__);
    p1=c1.positionBodyToWorld(Vector3(1.0,1.0,1.0));
    assertVector3Equal(p1, {1., 0., 1 + sqrt(2)}, __LINE__);
    p1=c1.directionWorldToBody(Vector3(1.0,1.0,1.0));
    assertVector3Equal(p1, {1., sqrt(2), 0.}, __LINE__);
    p1=c1.directionBodyToWorld(Vector3(1.0,1.0,1.0));
    assertVector3Equal(p1, {1., 0., sqrt(2)}, __LINE__);

    // 欧拉角测试
    Quaternion q2(PI/4., -PI/4., PI/2.);
    assertVector3Equal(q2.getEuler(), {PI/2., -PI/4., PI/4.}, __LINE__);
    Coordinate c2{{0., 0., 0.}, {q2.x, q2.y, q2.z}};
    assertVector3Equal(c2.positionWorldToBody({0.5, 0.5, sqrt(2.)/2.}), {1., 0., 0.}, __LINE__);
    assertVector3Equal(c2.positionWorldToBody({-0.5, -0.5, sqrt(2.)/2.}), {0., 1., 0.}, __LINE__);
    assertVector3Equal(c2.positionWorldToBody({sqrt(2.)/2., -sqrt(2.)/2., 0.}), {0., 0., 1.}, __LINE__);
    
    assertVector3Equal(c2.positionBodyToWorld({1., 0., 0.}), {0.5, 0.5, sqrt(2.)/2.}, __LINE__);
    assertVector3Equal(c2.positionBodyToWorld({0., 1., 0.}), {-0.5, -0.5, sqrt(2.)/2.}, __LINE__);
    assertVector3Equal(c2.positionBodyToWorld({0., 0., 1.}), {sqrt(2.)/2., -sqrt(2.)/2., 0.}, __LINE__);

    return 0;
}
}