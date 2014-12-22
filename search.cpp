// Bryan Jones and Melanie Subbiah (2014)
// All rights reserved

#include <stdlib.h>
#include "App.h"
#include "Mandelbulb.h"
#include "math3d.h"
#include "Search.h"
#include <cassert>
#include <cmath>

//Construct Search using the App constructor
Search::Search(std::string& windowCaption, int imageWidth, int imageHeight, float zoom, float exposureConstant, float imageGamma) : App(windowCaption, imageWidth, imageHeight, zoom, exposureConstant, imageGamma) {}

//Change the color of a pixel (takes x and y values)
void Search::setPixel(int x, int y, const Color& c) {
  setPixel(Vector2(x, y), c);
}

//Change the color of a pixel (takes a Vector2)
void Search::setPixel(Vector2 point, const Color& c) {
  m_imageData[m_imageWidth * point.y + point.x] = c;
}

//Retrieve a pixel (takes x and y values)
Color Search::pixel(int x, int y) const {
  return pixel(Vector2(x, y));
}

//Retrieve a pixel (takes a Vector2)
Color Search::pixel(Vector2 point) const {
  return m_imageData[m_imageWidth * point.y + point.x];
}

//Draws a line (takes x and y values)
void Search::drawLine(int x0, int y0, int x1, int y1, const Color& c) {
  drawLine(Vector2(x0, y0), Vector2(x1, y1), c);
}

//Draws a line (takes a Vector2)
void Search::drawLine(Vector2 point0, Vector2 point1, const Color& c) {
  //Check line endpoints are within display range
  if(point0.x >= 0 && point0.x < m_imageWidth && point0.y >= 0 && point0.y < m_imageHeight && point1.x >= 0 && point1.x < m_imageWidth && point1.y >= 0 && point1.y < m_imageHeight) {
 
    //Draw a vertical line 
    if(point0.x == point1.x) {
 
     //Make sure the second point has a larger y value than the first point
      if (point0.y > point1.y) {
	std::swap(point0, point1);
      }
      for(int y = point0.y; y <= point1.y; ++y) {
	setPixel(Vector2(int(point0.x), y), c);
      }

      //Draw a horizontal line
    } else {

      //Slope
      float m = (point1.y - point0.y)/(point1.x - point0.x);
      
      //Draw flatter lines
      if (std::abs(m) < 1) {
	if (point0.x > point1.x) {
	  std::swap(point0, point1);
	}
	int y = point0.y;
	for(int x = point0.x; x <= point1.x; ++x, y += m) {
	  setPixel(Vector2(x, y), c);
	}

	//Draw steeper lines
      } else {
	if (point0.y > point1.y) {
	  std::swap(point0, point1);
	}
	
	float x = float(point0.x);
	for(float y = point0.y; y <= point1.y; ++y, x += (1.0/m)) {
	  setPixel(Vector2(x, y), c);
	}
      }
    }
  }
}

//Draw a box (takes x and y values for the lower left and upper right corners)
void Search::drawBox(int x0, int y0, int x1, int y1, const Color& c) {
  drawBox(Vector2(x0, y0), Vector2(x1, y1), c);
}

//Draw a box (takes Vector2 values for the lower left and upper right corners)
void Search::drawBox(Vector2 point0, Vector2 point1, const Color& c) {
  //Determine remaining corners
  Vector2 upperLeft = Vector2(point0.x, point1.y);
  Vector2 lowerRight = Vector2(point1.x, point0.y);
  
  //Draw box lines
  drawLine(point0, upperLeft, c);
  drawLine(upperLeft, point1, c);
  drawLine(point1, lowerRight, c);
  drawLine(lowerRight, point0, c);
}

//Draw axes of graph
void Search::drawAxes( float hashMarks_x, float hashMarks_y, const Color& c) {
  //Draw the axes
  drawLine(Vector2(0, (m_imageHeight - 1)/2), Vector2( m_imageWidth - 1, (m_imageHeight - 1)/2), c); 
  drawLine(Vector2((m_imageWidth - 1)/2, 0), Vector2((m_imageWidth - 1)/2, m_imageHeight - 1), c);

  //Calculate space between HashSpaces
  float hashSpaces_x = float(m_imageWidth - 1)/hashMarks_x;

  //Draw X-axis HashSpaces
  for( float i = 0; i <= float(m_imageWidth - 1); i += hashSpaces_x) {
    drawLine(Vector2(i, (float(m_imageHeight - 1)/2) + 3), Vector2(i, (float(m_imageHeight - 1)/2) - 3), c);
  }

  //Draw Y-axis HashSpaces
  float hashSpaces_y = float(m_imageHeight - 1)/hashMarks_y;
  for( float i = 0; i <= float(m_imageHeight - 1); i += hashSpaces_y) {
    drawLine(Vector2(float(m_imageWidth - 1)/2 - 3, i), Vector2(float(m_imageWidth - 1)/2 + 3, i), c);
  }
}

//Plot a function
void Search::plot( Function& f, float domain_s, float domain_e, float range_s, float range_e, Color c, bool newton) {

  //Convert from pixels to axis values
  float inc = (domain_e - domain_s) / float(m_imageWidth - 1);

  //Plot the function by drawing lines from (x, f(x)) to (x+1, f(x+1))
  for( float i = domain_s; i <= (domain_e); i += inc) { 
    // x-value is shifted right one pixel to align with axes
    drawLine(Vector2(int(i/inc + float((m_imageWidth - 1)/2)) + 1, int(float((m_imageHeight - 1)/2) - f(i)/inc)), Vector2(int((i + inc)/inc + float((m_imageWidth - 1)/2)), int(float((m_imageHeight - 1)/2) - f(i + inc)/inc)), c);
  }

  //Find the roots using linear search and then either binary search or Newton's method
  std::vector<float> root;
  if( newton == true ) {
    findRoots_N(f, domain_s, domain_e, root);
  } else {
    findRoots(f, domain_s, domain_e, root);
  }

  //Draw boxes around the roots
  while( root.size() > 0) {
    Vector2 one(float((m_imageWidth - 1)/2 + root.front()/inc - 3) , float((m_imageHeight - 1)/2) + 3);
    Vector2 two(float((m_imageWidth - 1)/2 + root.front()/inc + 4) , float((m_imageHeight - 1)/2) - 3);

    drawBox(one, two, Color(0, 250, 250));
    root.erase(root.begin());
  }
}

//Find roots using coarse linear search and then binary search
void Search::findRoots( const Function& f, float xMin, float xMax, std::vector<float>& root) const {
  //Increment for linear search
  float inc = 0.2f;

  //Use binary search once a potential location for a root is found
  for( float i = xMin; i <= xMax; i += inc) {
    if( (f(i) > 0 && f(i + inc) < 0) || (f(i) < 0 && f(i + inc) > 0) ) {
      root.push_back(binarySearch(f, i, i + inc, 80));
    }
  }
}

//Find roots using Newton's method
void Search::findRoots_N( const Function& f, float xMin, float xMax, std::vector<float>& root) const {
  //Increment for linear search
  float inc = 0.3f;

  //Use Newton's method once a potential location for a root is found
  for( float i = xMin; i <= xMax; i += inc) {
    if( (f(i) > 0 && f(i + inc) < 0) || (f(i) < 0 && f(i + inc) > 0) ) {
      Derivative d(f);
      root.push_back(newtonSearch(f, d, i));
    }
  }
}

//Use Newton's method to find a root
float Search::newtonSearch( const Function& f, const Derivative& d, float x, float err) const {
  //Calculate the value of the derivative of x
  float m = d(x);
  float b = f(x) - m * x;
  x = -b / m;

  //Check whether the point is a zero
  if( std::abs(f(x)) <= err) {
    return x;
  } else {
    return newtonSearch(f, d, x);
  }
} 

//Use binary search to find a root
float Search::binarySearch( const Function& f, float xMin, float xMax, int iterations) const {
  //Error threshold
  float err = 0.0003f;
  
  //Calculate the midpoint
  float mid = (xMax + xMin) / 2.0f;

  //Determine whether the point is a zero and if not, which side of the midpoint should be searched for a root
  if( std::fabs(f(mid)) <= err || iterations == 0) {
    return mid;
  } else {
    --iterations;
    if( ( f(mid) < 0 && f(xMin) < 0) || ( f(mid) > 0 && f(xMin) > 0 )) {
      return binarySearch(f, mid, xMax, iterations);
    } else {
      return binarySearch(f, xMin, mid, iterations);
    }
  }
}

//Exit if escape is pressed, otherwise save and exit
void Search::onKeyPress( unsigned char key) {
  if( key != 27 ) {
    saveImage("MyMasterpiece.tga");
  }
    ::exit(0);
}

//Find the smallest root of a distance function to draw 3D shapes
float Search::findSmallestRootOfDistanceFunction(const Function& f, float xMin, float xMax) const {
  //Initialize values
  float x = xMin;
  float last = x;

  //Minimum step
  float inc = 1e-3;
  
  //Error threshold
  float threshold = minimumDistanceToSurface;
  
  //Approach the surface
  while( f(x) > 0 && x <= xMax ) {
    last = x;
    x = x + max( f(x), inc);
  }

  //Save highest value attempted
  float highest = x;

  //Use binary search to find root
  if( x <= xMax ) {
     x = binarySearch(f, last, highest, 80); 
  }

  //Return the root
  if( f(x) < threshold) {
    return x;
  } else {
    return NAN;
  }
}

//Determines what is drawn on the image
void Search::onGraphics() {
  /*
  drawAxes( 8, 8, Color( 1, 1, 1));
  Cubic f( -1,0, 1, 0);
  plot( f, -4, 4, -4, 4, Color( 1, 0, 0), false);
  */
  
  /*
  RoundBox myRoundBox;
  myRoundBox.setRotation( 0.1*frame, 0.1*frame, 0.1*frame);
  ++frame;
  */
  
  Mandelbulb mandelbulb(6.0f);
  mandelbulb.setRotation(0.5, 0.5, 0.5);
  drawRayCastImage( mandelbulb, 3.0f);  
}
