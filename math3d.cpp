#include "math3d.h"
#include <algorithm>

Quaternion Quaternion::slerp(Quaternion const& q1, Quaternion const& q2, float t) {
  float dot = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
  if (dot < -1.0f + 1e-6f || dot > 1.0f - 1e-6f) return q1;
  float angle = acos(dot);
  float isin = 1.0f / sin(angle);
  float s1 = isin * sin((1.0f - t) * angle);
  float s2 = isin * sin(t * angle);
  return Quaternion(s1 * q1.x + s2 * q2.x,
                    s1 * q1.y + s2 * q2.y,
                    s1 * q1.z + s2 * q2.z,
                    s1 * q1.w + s2 * q2.w);
}

float Matrix::det() const {
  return (m00 * m11 - m01 * m10) * (m22 * m33 - m23 * m32) +
         (m00 * m12 - m02 * m10) * (m21 * m33 - m23 * m31) +
         (m00 * m13 - m03 * m10) * (m21 * m32 - m22 * m31) +
         (m01 * m12 - m02 * m11) * (m20 * m33 - m23 * m30) +
         (m01 * m13 - m03 * m11) * (m20 * m32 - m22 * m30) +
         (m02 * m13 - m03 * m12) * (m20 * m31 - m21 * m30);
}
Matrix Matrix::inverse() const {
  Matrix lhs = *this;
  Matrix rhs = Matrix::identity();
  for (int i = 0; i < 4; ++i) {
    int next = i;
    for (int j = i + 1; j < 4; ++j) if (fabs(lhs.m[j][i]) > fabs(lhs.m[next][i])) next = j;
    if (fabs(lhs.m[next][i]) < 1e-6) return Matrix();
    if (next != i) {
      std::swap(lhs.m[next], lhs.m[i]);
      std::swap(rhs.m[next], rhs.m[i]);
    }
    float c = lhs.m[i][i];
    for (int j = 0; j < 4; ++j) {
      lhs.m[i][j] /= c;
      rhs.m[i][j] /= c;
    }
    for (int j = i + 1; j < 4; ++j) {
      float c = lhs.m[j][i];
      for (int k = 0; k < 4; ++k) {
        lhs.m[j][k] -= lhs.m[i][k] * c;
        rhs.m[j][k] -= rhs.m[i][k] * c;
      }
    }
  }
  for (int i = 3; i >= 0; --i) {
    for (int j = i + 1; j < 4; ++j) {
      float c = lhs.m[i][j];
      for (int k = 0; k < 4; ++k) {
        rhs.m[i][k] -= rhs.m[j][k] * c;
      }
    }
  }
  return rhs;
}
