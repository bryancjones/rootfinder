// Bryan Jones and Melanie Subbiah (2014)
// All rights reserved

#ifndef Search_h
#define Search_h
#include <stdio.h>
#include "App.h"
#include "Mandelbulb.h"
#include "math3d.h"

class Derivative : public Function {

 private:
  const Function& f;
  float h;
 public:

  Derivative(const Function& f, float h = 1e-3f) : f(f), h(h) {}
  virtual float operator()(float x) const override {
    return (f(x+h) - f(x-h)) / (2.0f * h);
  }
};


class Search : public App {
 private:

  int frame = 0;


 public:

  Search(std::string& windowCaption, int imageWidth, int imageHeight, float zoom = 2.0f, float exposureconstant = 1.0f, float imageGamma = 2.1f);

  virtual void setPixel(int x, int y, const Color& c) override;

  void setPixel(Vector2 point, const Color& c);

  virtual Color pixel(int x, int y) const override;

  Color pixel(Vector2 point) const;

  virtual void drawLine(int x0, int y0, int x1, int y1, const Color& c) override;

  void drawLine(Vector2 point0, Vector2 point1, const Color& c);
  virtual void drawBox(int x0, int y0, int x1, int y1, const Color& c) override;

  void drawBox(Vector2 point0, Vector2 point1, const Color& c);

  virtual void onGraphics() override;

  void drawAxes( float hashMarks_x, float hashMarks_y, const Color& c);

  void plot( Function& f, float domain_s, float domain_e, float range_s, float range_e, Color c, bool newton = false);

  virtual void findRoots( const Function& f, float xMin, float xMax, std::vector<float>& root) const override;

  float binarySearch( const Function& f, float xMin, float xMax, int iterations) const;

  virtual float findSmallestRootOfDistanceFunction(const Function& f, float xMin, float xMax) const override;

  virtual void onKeyPress( unsigned char key) override;

  void findRoots_N( const Function& f, float xMin, float xMax, std::vector<float>& root) const;

  float newtonSearch( const Function& f, const Derivative& d, float x, float err = 1e-5f) const;

};

class Cubic : public Function {
protected:
  float a, b, c, d;

public:

 Cubic(float a, float b, float c, float d) : a(a), b(b), c(c), d(d) {}

    virtual float operator()(float x) const override {
        return a * x * x * x + b * x * x + c * x + d;
    }

};

#endif
