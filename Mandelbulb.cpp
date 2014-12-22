// Code taken from http://cs.williams.edu/~morgan/cs136/schedule.html

#include "Mandelbulb.h"
#include "App.h"

void RoundBox::getDistanceAndShade(const Point3& point, float& distance, float& shade) const {
    // Rotate the query point into the reference frame of the function
    // 3x3 matrix-vector product
    const Point3& P = rotation * point;
    shade = 1.0f;

    // Sample distance function for a sphere:
    // distance = length(P) - 0.5f; return;
    
    const float side = 0.5f;
    // Unit rounded box (http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm)
    distance = length(max(abs(P) - Vector3(1.0f, 1.0f, 1.0f) * side, Vector3(0.0f, 0.0f, 0.0f))) - 0.1f * side;
}

///////////////////////////////////////

Mandelbulb::Mandelbulb(float power) : power(power) {}


void Mandelbulb::getDistanceAndShade(const Point3& point, float& distance, float& shade) const {
    // Rotate the query point into the reference frame of the function
    // 3x3 matrix-vector product
    const Point3& P = rotation * point;
    shade = 1.0f;
    
    // This is a 3D analog of the 2D Mandelbrot set. Altering the mandlebulbExponent
    // affects the shape.
    // See the equation at
    // http://blog.hvidtfeldts.net/index.php/2011/09/distance-estimated-3d-fractals-v-the-mandelbulb-different-de-approximations/
    Point3 Q = P;
    
    // Put the whole shape in a bounding sphere to 
    // speed up distant ray marching. This is necessary
    // to ensure that we don't expend all ray march iterations
    // before even approaching the surface
    {
        const float externalBoundingRadius = 1.2f;
        distance = length(P) - externalBoundingRadius;
        // If we're more than 1 unit away from the
        // surface, return that distance
        if (distance > 1.0f) { return; }
    }

    // Used to smooth discrete iterations into continuous distance field
    // (similar to the trick used for coloring the Mandelbrot set)
    float derivative = 1.0f;

    // Higher is more complex and fills holes
    const int ITERATIONS = 18;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        // Darken as we go deeper
        shade *= 0.725f;
        const float r = length(Q);
        
        if (r > 2.0f) {
            // The point escaped. Remap shade for more brightness and return
            shade = min((shade + 0.075f) * 4.1f, 1.0f);

            // Bias slightly so that our root finder can identify a true zero
            distance = 0.5f * log(r) * r / derivative - 0.001f;
            return;
        } else {
            // Convert to polar coordinates and then rotate by the power
            const float theta = acos(Q.z / r) * power;
            const float phi   = atan2(Q.y, Q.x) * power;
            
            // Update the derivative
            derivative = pow(r, power - 1.0f) * power * derivative + 1.0f;
            
            // Convert back to Cartesian coordinates and 
            // offset by the original point (which we're orbiting)
            const float sinTheta = sin(theta);
            
            Q = Vector3(sinTheta * cos(phi),
                        sinTheta * sin(phi),
                        cos(theta)) * pow(r, power) + P;
        }
    }
    
    // Never escaped, so either already in the set...or a complete miss
    distance = App::minimumDistanceToSurface;
}
