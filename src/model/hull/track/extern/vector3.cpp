// #include "vector3.h"

// namespace externModel::track{

// Quaternion::Quaternion(const Vector33& axis, element angle){
//     using std::sin;
//     using std::cos;
//     Vector33 normlized = axis/sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
//     element sin_a = sin(angle / 2);
//     element cos_a = cos(angle / 2);
//     x = normlized.x * sin_a;
//     y = normlized.y * sin_a;
//     z = normlized.z * sin_a;
//     w = cos_a;
// };

// Vector33 Vector33::rotate(const Vector33& p) const{
//     Quaternion q(*this, true);
//     Quaternion pq(p, false);
//     Quaternion r = q.rotate(pq);
//     return Vector33(r.x, r.y, r.z);
// }

// }