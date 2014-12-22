// Code taken from http://cs.williams.edu/~morgan/cs136/schedule.html

#ifndef math3d_h
#define math3d_h

#include <cmath>
#include <algorithm>

using std::min;
using std::max;
using std::cos;
using std::sin;
using std::atan2;
using std::acos;
using std::asin;
using std::abs;
using std::pow;

// Methods are intentionally implemented in the header file to allow
// the compiler to inline them for performance.

class Color {
 public:
  float       r;
  float       g;
  float       b;
    
 Color() : r(0), g(0), b(0) {}

 Color(float r, float g, float b) : r(r), g(g), b(b) {}

  explicit Color(const class Vector3& c);

  Color operator*(float k) const {
    return Color(r * k, g * k, b * k);
  }

  Color operator/(float k) const {
    return Color(r / k, g / k, b / k);
  }

  Color operator*(const Color& c) const {
    return Color(r * c.r, g * c.g, b * c.b);
  }

  Color operator/(const Color& c) const {
    return Color(r / c.r, g / c.g, b / c.b);
  }

  Color operator+(const Color& c) const {
    return Color(r + c.r, g + c.g, b + c.b);
  }

  Color operator-(const Color& c) const {
    return Color(r - c.r, g - c.g, b - c.b);
  }

  static Color black() { return Color(0, 0, 0); }
  static Color red()   { return Color(1, 0, 0); }
  static Color yellow(){ return Color(1, 1, 0); }
  static Color green() { return Color(0, 1, 0); }
  static Color cyan()  { return Color(0, 1, 1); }
  static Color blue()  { return Color(0, 0, 1); }
  static Color white() { return Color(1, 1, 1); }
};


class Vector2 {
 public:
  float x, y;

 Vector2() : x(0), y(0) {}

 Vector2(float x, float y) : x(x), y(y) {}

 Vector2(int x, int y) : x(float(x)), y(float(y)) {}

  Vector2 operator*(const float k) const {
    return Vector2(x * k, y * k);
  }

  Vector2 operator/(const float k) const {
    return Vector2(x / k, y / k);
  }

  Vector2 operator*(const Vector2& v) const {
    return Vector2(x * v.x, y * v.y);
  }

  Vector2 operator/(const Vector2& v) const {
    return Vector2(x / v.x, y / v.y);
  }

  Vector2 operator+(const Vector2& P) const {
    return Vector2(x + P.x, y + P.y);
  }

  Vector2 operator-(const Vector2& P) const {
    return Vector2(x - P.x, y - P.y);
  }
};


class Vector3 {
 public:
  float x, y, z;

 Vector3() : x(0), y(0), z(0) {}

 Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

 Vector3(int x, int y, int z) : x(float(x)), y(float(y)), z(float(z)) {}

  explicit Vector3(const class Color& c);

  Vector3 operator*(const float k) const {
    return Vector3(x * k, y * k, z * k);
  }

  Vector3 operator/(const float k) const {
    return Vector3(x / k, y / k, z / k);
  }

  Vector3 operator/(const Vector3& v) const {
    return Vector3(x / v.x, y / v.y, z / v.z);
  }

  Vector3 operator*(const Vector3& v) const {
    return Vector3(x * v.x, y * v.y, z * v.z);
  }

  Vector3 operator+(const Vector3& P) const {
    return Vector3(x + P.x, y + P.y, z + P.z);
  }

  Vector3 operator-(const Vector3& P) const {
    return Vector3(x - P.x, y - P.y, z - P.z);
  }

  /* Returns a 2-vector of the x and y components */
  Vector2 xy() const {
    return Vector2(x, y);
  }

  /* Returns a 2-vector whose y component is the z component of this
     vector. This is a "swizzle" operation. */
  Vector2 xz() const {
    return Vector2(x, z);
  }

  Vector2 yz() const {
    return Vector2(y, z);
  }
};


class Matrix3x3 {
 public:
  /* row-column zero-based indexed */
  float element[3][3];

  /* Initializes to the identity matrix */
  Matrix3x3() {
    for (int r = 0; r < 3; ++r) {
      for (int c = 0; c < 3; ++c) {
	element[r][c] = (r == c) ? 1.0f : 0.0f;
      }
    }            
  }
    
  Matrix3x3(float e00, float e01, float e02,
	    float e10, float e11, float e12,
	    float e20, float e21, float e22) {
    element[0][0] = e00; element[0][1] = e01; element[0][2] = e02;
    element[1][0] = e10; element[1][1] = e11; element[1][2] = e12;
    element[2][0] = e20; element[2][1] = e21; element[2][2] = e22;
  }

  Matrix3x3 operator*(const Matrix3x3& M) const {

    Matrix3x3 C;



    for (int r = 0; r < 3; ++r) {

      for (int c = 0; c < 3; ++c) {

	C.element[r][c] = 0.0f;

	for (int i = 0; i < 3; ++i) {

	  C.element[r][c] += element[r][i] * M.element[i][c];

	}

      }

    }



    return C;

  }
  Vector3 operator*(const Vector3& v) const {
    return Vector3(element[0][0] * v.x + element[0][1] * v.y + element[0][2] * v.z,
		   element[1][0] * v.x + element[1][1] * v.y + element[1][2] * v.z,
		   element[2][0] * v.x + element[2][1] * v.y + element[2][2] * v.z);
  }

};


inline Vector3::Vector3(const Color& c) : x(c.r), y(c.g), z(c.b) {}


inline Color::Color(const Vector3& v) : r(v.x), g(v.y), b(v.z) {}


inline Vector2 operator*(float f, Vector2 v) {
  return v * f;
}


inline Vector3 max(const Vector3& a, const Vector3& b) {
  return Vector3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}


inline Vector3 operator*(float f, Vector3 v) {
  return v * f;
}


inline Color operator*(float f, Color v) {
  return v * f;
}


inline Color pow(const Color& c, float k) {
  return Color(::powf(c.r, k), ::powf(c.g, k), ::powf(c.b, k));
}


inline Vector2 pow(const Vector2& c, float k) {
  return Vector2(::powf(c.x, k), ::powf(c.y, k));
}


inline Vector3 pow(const Vector3& c, float k) {
  return Vector3(::powf(c.x, k), ::powf(c.y, k), ::powf(c.z, k));
}



/* Computes the dot (inner) product */
inline float dot(const Vector2& a, const Vector2& v) {
  return a.x * v.x + a.y * v.y;
} 


/* Computes the dot (inner) product */
inline float dot(const Vector3& a, const Vector3& v) {
  return a.x * v.x + a.y * v.y + a.z * v.z;
}


/* Computes the dot (inner) product */
inline float dot(const Color& a, const Color& b) {
  return a.r * b.r + a.g * b.g + a.b * b.b;
}


inline Color sqrt(const Color& c) {
  return Color(::sqrt(c.r), ::sqrt(c.g), ::sqrt(c.b));
}


inline Color abs(const Color& a) {
  return Color(::fabsf(a.r), ::fabsf(a.g), ::fabsf(a.b));
}


inline Vector2 abs(const Vector2& a) {
  return Vector2(::fabsf(a.x), ::fabsf(a.y));
}


inline Vector3 abs(const Vector3& a) {
  return Vector3(::fabsf(a.x), ::fabsf(a.y), ::fabsf(a.z));
}


inline float clamp(float a, float lo, float hi) {
  return max(min(a, hi), lo);
}

/* Linear interpolation (see GLSL "mix") */
template<class T>
inline T mix(const T& a, const T& b, float t) {
  return a + (b - a) * t;
}

/* Vector magnitude (L^2 norm) */
template<class T>
inline float length(const T& v) {
  return sqrt(dot(v, v));
}


/* Create a unit vector */
template<class T>
inline T normalize(const T& v) {
  return v * (1.0f / length(v));
}

/* Points are locations in space. Vectors are the distance and
   direction between them */
typedef Vector3 Point3;


/* Points are locations in space. Vectors are the distance and
   direction between them */
typedef Vector2 Point2;


/* Base class for distance functions like Mandelbulb */
class Shape {
 protected:

  /* 3x3 rotation matrix http://en.wikipedia.org/wiki/Rotation_matrix#In_three_dimensions */
  Matrix3x3 rotation;
    
 public:
 
  /* Computes a conservative estimate of the distance from point to the surface of the object
     and a 0 <= shade <= 1 that is lower in cracks and higher at ridges.  */
  virtual void getDistanceAndShade(const Point3& point, float& distance, float& shade) const = 0;

  // Objects that can have subclasses must have virtual destructors in C++
  virtual ~Shape() {}
  virtual float distance(const Point3& point) const;
  virtual float shade(const Point3& point) const;
  void setRotation(float yaw, float pitch, float roll);
};


/** An arbitrary function from reals -> reals */
class Function {
 public:
  virtual ~Function() {}
  virtual float operator()(float x) const = 0;
};

/** A generic real quadratic function */
class Quadratic : public Function {
 protected:
  float a, b, c;

 public:

 Quadratic(float a, float b, float c) : a(a), b(b), c(c) {}

  virtual float operator()(float x) const override {
    return a * x * x + b * x + c;
  }

};


class DistanceToShapeOnRay : public Function {
 private:
  Point3  origin;
  Vector3 direction;
  const Shape&  shape;

 public:
 DistanceToShapeOnRay(const Point3& P, const Vector3& v, const Shape& s) : origin(P), direction(v), shape(s) {}
  virtual float operator()(float f) const override {
    return shape.distance(origin + direction * f);
  }
};


#endif 
