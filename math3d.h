#pragma once
#include <cmath>
#include <cstring>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif

class Vector {
public:
  float x, y, z;

  Vector(float x = 0, float y = 0, float z = 0)
    : x(x), y(y), z(z)
  {}

  float& operator[](int i) {
    return (&x)[i];
  }
  float const& operator[](int i) const {
    return (&x)[i];
  }
  operator float*() {
    return &x;
  }
  operator float const*() const {
    return &x;
  }

  float length() const {
    return sqrt(x * x + y * y + z * z);
  }
  float length2() const {
    return x * x + y * y + z * z;
  }

  friend Vector operator+(Vector const& lhs, Vector const& rhs) {
    return Vector(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
  }
  friend Vector operator-(Vector const& lhs, Vector const& rhs) {
    return Vector(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
  }
  friend Vector operator*(Vector const& lhs, float rhs) {
    return Vector(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
  }
  friend Vector operator*(float lhs, Vector const& rhs) {
    return Vector(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
  }
  friend Vector operator/(Vector const& lhs, float rhs) {
    return Vector(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
  }

  Vector operator-() const {
    return Vector(-x, -y, -z);
  }
  Vector operator+() const {
    return Vector(x, y, z);
  }
  Vector& operator+=(Vector const& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }
  Vector& operator-=(Vector const& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }
  Vector& operator*=(float rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
  }
  Vector& operator/=(float rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
  }

  friend float operator&(Vector const& lhs, Vector const& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
  }
  friend Vector operator^(Vector const& lhs, Vector const& rhs) {
    return Vector(
      lhs.y * rhs.z - lhs.z * rhs.y,
      lhs.z * rhs.x - lhs.x * rhs.z,
      lhs.x * rhs.y - lhs.y * rhs.x
    );
  }

  Vector& normalize() {
    float len = length();
    x /= len;
    y /= len;
    z /= len;
    return *this;
  }
  Vector normalized() const {
    return (*this) / length();
  }

  Vector perp() const {
    float ax = fabs(x), ay = fabs(y), az = fabs(z);
    if (ax < ay && ax < az) {
      return Vector(1, 0, 0) ^ (*this);
    } else if (ay < az) {
      return Vector(0, 1, 0) ^ (*this);
    } else {
      return Vector(0, 0, 1) ^ (*this);
    }
  }
};

class Matrix {
public:
  union {
    struct {
      float m00, m01, m02, m03;
      float m10, m11, m12, m13;
      float m20, m21, m22, m23;
      float m30, m31, m32, m33;
    };
    float m[4][4];
    float M[16];
  };

  Matrix() {
    memset(M, 0, sizeof M);
  }
  Matrix(float m00, float m01, float m02, float m03,
         float m10, float m11, float m12, float m13,
         float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33)
    : m00(m00), m01(m01), m02(m02), m03(m03)
    , m10(m10), m11(m11), m12(m12), m13(m13)
    , m20(m20), m21(m21), m22(m22), m23(m23)
    , m30(m30), m31(m31), m32(m32), m33(m33)
  {}

  float& operator[](int i) {
    return M[i];
  }
  float const& operator[](int i) const {
    return M[i];
  }
  float& operator()(int i, int j) {
    return m[i][j];
  }
  float const& operator()(int i, int j) const {
    return m[i][j];
  }
  operator float*() {
    return M;
  }
  operator float const*() const {
    return M;
  }

  float det() const;
  Matrix inverse() const;
  Matrix& invert() {
    return *this = inverse();
  }

  friend Matrix operator+(Matrix const& lhs, Matrix const& rhs) {
    Matrix res;
    for (size_t i = 0; i < 16; ++i) res.M[i] = lhs.M[i] + rhs.M[i];
    return res;
  }
  friend Matrix operator-(Matrix const& lhs, Matrix const& rhs) {
    Matrix res;
    for (size_t i = 0; i < 16; ++i) res.M[i] = lhs.M[i] - rhs.M[i];
    return res;
  }
  friend Matrix operator*(Matrix const& lhs, float rhs) {
    Matrix res;
    for (size_t i = 0; i < 16; ++i) res.M[i] = lhs.M[i] * rhs;
    return res;
  }
  friend Matrix operator*(float lhs, Matrix const& rhs) {
    Matrix res;
    for (size_t i = 0; i < 16; ++i) res.M[i] = lhs * rhs.M[i];
    return res;
  }
  friend Matrix operator/(Matrix const& lhs, float rhs) {
    Matrix res;
    for (size_t i = 0; i < 16; ++i) res.M[i] = lhs.M[i] / rhs;
    return res;
  }
  friend Matrix operator*(Matrix const& lhs, Matrix const& rhs) {
    Matrix res;
    for (size_t i = 0; i < 4; ++i) for (size_t j = 0; j < 4; ++j) {
      res.m[i][j] = lhs.m[i][0] * rhs.m[0][j] + lhs.m[i][1] * rhs.m[1][j] + lhs.m[i][2] * rhs.m[2][j] + lhs.m[i][3] * rhs.m[3][j];
    }
    return res;
  }

  Matrix operator+() const {
    return *this;
  }
  Matrix operator-() const {
    return Matrix(-m00, -m01, -m02, -m03,
                  -m10, -m11, -m12, -m13,
                  -m20, -m21, -m22, -m23,
                  -m30, -m31, -m32, -m33);
  }

  Matrix& operator+=(Matrix const& rhs) {
    for (size_t i = 0; i < 16; ++i) M[i] += rhs.M[i];
    return *this;
  }
  Matrix& operator-=(Matrix const& rhs) {
    for (size_t i = 0; i < 16; ++i) M[i] -= rhs.M[i];
    return *this;
  }
  Matrix& operator*=(float rhs) {
    for (size_t i = 0; i < 16; ++i) M[i] *= rhs;
    return *this;
  }
  Matrix& operator/=(float rhs) {
    for (size_t i = 0; i < 16; ++i) M[i] /= rhs;
    return *this;
  }
  Matrix& operator*=(Matrix const& rhs) {
    return (*this) = (*this) * rhs;
  }

  Vector row(int i) const {
    return Vector(m[i][0], m[i][1], m[i][2]);
  }
  Vector col(int i) const {
    return Vector(m[0][i], m[1][i], m[2][i]);
  }
  Matrix& row(int i, Vector const& v) {
    m[i][0] = v.x;
    m[i][1] = v.y;
    m[i][2] = v.z;
    return *this;
  }
  Matrix& col(int i, Vector const& v) {
    m[0][i] = v.x;
    m[1][i] = v.y;
    m[2][i] = v.z;
    return *this;
  }

  friend Vector operator*(Matrix const& lhs, Vector const& rhs) {
    return Vector(lhs.m00 * rhs.x + lhs.m01 * rhs.y + lhs.m02 * rhs.z + lhs.m03,
                  lhs.m10 * rhs.x + lhs.m11 * rhs.y + lhs.m12 * rhs.z + lhs.m13,
                  lhs.m20 * rhs.x + lhs.m21 * rhs.y + lhs.m22 * rhs.z + lhs.m23);
  }

  Vector apply(Vector const& v) const {
    float w = m30 * v.x + m31 * v.y + m32 * v.z + m33;
    return Vector(m00 * v.x + m01 * v.y + m02 * v.z + m03,
                  m10 * v.x + m11 * v.y + m12 * v.z + m13,
                  m20 * v.x + m21 * v.y + m22 * v.z + m23) / w;
  }
  Vector rotate(Vector const& v) const {
    return Vector(m00 * v.x + m01 * v.y + m02 * v.z,
                  m10 * v.x + m11 * v.y + m12 * v.z,
                  m20 * v.x + m21 * v.y + m22 * v.z);
  }

  Matrix transposed() const {
    return Matrix(m00, m10, m20, m30,
                  m01, m11, m21, m31,
                  m02, m12, m22, m32,
                  m03, m13, m23, m33);
  }
  Matrix& transpose() {
    return *this = Matrix(m00, m10, m20, m30,
                          m01, m11, m21, m31,
                          m02, m12, m22, m32,
                          m03, m13, m23, m33);
  }

  static Matrix identity() {
    return Matrix(1, 0, 0, 0,
                  0, 1, 0, 0,
                  0, 0, 1, 0,
                  0, 0, 0, 1);
  }
  static Matrix scale(float x, float y, float z) {
    return Matrix(x, 0, 0, 0,
                  0, y, 0, 0,
                  0, 0, z, 0,
                  0, 0, 0, 1);
  }
  static Matrix scale(float f) {
    return scale(f, f, f);
  }
  static Matrix translate(float x, float y, float z) {
    return Matrix(1, 0, 0, x,
                  0, 1, 0, y,
                  0, 0, 1, z,
                  0, 0, 0, 1);
  }
  static Matrix translate(Vector const& v) {
    return translate(v.x, v.y, v.z);
  }
  static Matrix rows(Vector const& x, Vector const& y, Vector const& z, Vector const& w = Vector()) {
    return Matrix(x.x, x.y, x.z, 0,
                  y.x, y.y, y.z, 0,
                  z.x, z.y, z.z, 0,
                  w.x, w.y, w.z, 1);
  }
  static Matrix rotate(Vector const& v, float phi) {
    float c = cos(phi), s = sin(phi);
    return Matrix(v.x * v.x + (v.y * v.y + v.z * v.z) * c, v.y * v.x * (1 - c) + v.z * s,           v.z * v.x * (1 - c) - v.y * s,           0,
                  v.x * v.y * (1 - c) - v.z * s,           v.y * v.y + (v.x * v.x + v.z * v.z) * c, v.z * v.y * (1 - c) + v.x * s,           0,
                  v.x * v.z * (1 - c) + v.y * s,           v.y * v.z * (1 - c) - v.x * s,           v.z * v.z + (v.x * v.x + v.y * v.y) * c, 0,
                  0,                                       0,                                       0,                                       1);
  }
};

class Quaternion {
public:
  union {
    struct { Vector v; };
    struct { float x, y, z; };
  };
  float w;

  Quaternion(float x = 0, float y = 0, float z = 0, float w = 0)
    : x(x), y(y), z(z), w(w)
  {}
  Quaternion(Vector const& v, float w = 0)
    : v(v), w(w)
  {}

  Quaternion(float angle, Vector const& v) {
    float c = cos(angle * 0.5f), s = sin(angle * 0.5f);
    x = v.x * s;
    y = v.y * s;
    z = v.z * s;
    w = c;
  }

  Quaternion operator+() const {
    return *this;
  }
  Quaternion operator-() const {
    return Quaternion(-x, -y, -z, -w);
  }
  Quaternion conj() const {
    return Quaternion(-x, -y, -z, w);
  }

  friend Quaternion operator+(Quaternion const& lhs, Quaternion const& rhs) {
    return Quaternion(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z * rhs.z, lhs.w + rhs.w);
  }
  friend Quaternion operator-(Quaternion const& lhs, Quaternion const& rhs) {
    return Quaternion(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z * rhs.z, lhs.w + rhs.w);
  }
  friend Quaternion operator*(Quaternion const& lhs, Quaternion const& rhs) {
    return Quaternion(lhs.y * rhs.z - lhs.z * rhs.y + lhs.w * rhs.x + lhs.x * rhs.w,
                      lhs.z * rhs.x - lhs.x * rhs.z + lhs.w * rhs.y + lhs.y * rhs.w,
                      lhs.x * rhs.y - lhs.y * rhs.x + lhs.w * rhs.z + lhs.z * rhs.w,
                      lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z);
  }
  
  Quaternion& operator+=(Quaternion const& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
  }
  Quaternion& operator-=(Quaternion const& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
  }
  Quaternion& operator*=(Quaternion const& rhs) {
    return (*this) = (*this) * rhs;
  }

  Vector rotate(Vector const& v) {
    return (*this * v * conj()).v;
  }

  Quaternion& normalize() {
    float len2 = x * x + y * y + z * z;
    if (len2 > 1.0f - 1e-6f) {
      float inv = 1.0f / sqrt(len2);
      x *= inv;
      y *= inv;
      z *= inv;
      w = 0.0f;
    } else {
      w = sqrt(1.0f - len2);
    }
    return *this;
  }

  static Quaternion slerp(Quaternion const& a, Quaternion const& b, float t);

  Matrix matrix() const {
    return Matrix(1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (z * x + y * w), 0,
                  2 * (x * y + z * w), 1 - 2 * (z * z + x * x), 2 * (y * z - x * w), 0,
                  2 * (z * x - y * w), 2 * (y * z + x * w), 1 - 2 * (y * y + x * x), 0,
                  0, 0, 0, 1);
  }
};
