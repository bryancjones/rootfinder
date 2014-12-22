// Code taken from http://cs.williams.edu/~morgan/cs136/schedule.html

#ifndef Mandelbulb_h
#define Mandelbulb_h

#include "math3d.h"

class Mandelbulb : public Shape {
protected:

    /* Different values give different shapes; 8.0 is the "standard" bulb */
    float power;

public:
    
    Mandelbulb(float power = 8.0f);

    virtual void getDistanceAndShade(const Point3& point, float& distance, float& shade) const override;
};


class RoundBox : public Shape {
public:

    virtual void getDistanceAndShade(const Point3& point, float& distance, float& shade) const override;
};

#endif
