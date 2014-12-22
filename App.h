/**
   Application infrastructure for platform-independent real-time CPU rendering.
   Code taken from http://cs.williams.edu/~morgan/cs136/schedule.html
 */
#ifndef App_h
#define App_h
#include <vector>
#include <string>
#include "math3d.h"

/** Subclass this to create your own application */
class App {
public:

    /* A point this close to the surface is considered to be on the surface.
       Larger numbers lead to faster convergence but "blur" out the shape */
    static const float minimumDistanceToSurface;

private:
    
    /** For glut callbacks */
    static App*        instance;

    // The glut callbacks
    static void staticOnGraphics();
    static void staticOnKeyPress(unsigned char key, int x, int y);
    static void staticOnMousePress(int button, int state, int x, int y);
    static void staticOnMouseMotion(int x, int y);
    static void staticReshape(int width, int height);
    static void timerCallback(int value);

    const std::string  m_windowCaption;
    const float        m_zoom;
    const float        m_exposureConstant;
    const float        m_imageGamma;
    const int          m_frameTimeMilliseconds;

    void savePPM(const std::string& filename) const;
    void saveTGA(const std::string& filename) const;

protected:

    /** Row-major */
    std::vector<Color> m_imageData;
    const int          m_imageWidth;
    const int          m_imageHeight;

    int                m_mouseX;
    int                m_mouseY;

    // 3D graphics parameters:
    Vector3            m_lightDirection;

    Color              m_keyLightColor;
    Color              m_fillLightColor;

    Color              m_backgroundGradientCenterColor;
    Color              m_backgroundGradientRimColor;

    /* Shading of the surface as a function of height on [0, 1] */
    virtual Color surfaceColor(float height) const {
        return Color::white();
    }

    /* zoom is the amount to zoom the 3D image, different from m_zoom for 2D scaling of pixels */
    void drawRayCastImage(const Shape& shape, float zoom);

    /* Called from drawRayCastImage() for each pixel. Coord should be the center of the pixel. */
    Color computeRayCastPixel(const Point2 coord, const Shape& shape, float zoom);

    /* Called from computeRayCastPixel() for each sample within the pixel. (0.5, 0.5) is the center
       of the top-left pixel.*/
    Color computeRayCastSample(const Point2 coord, const Shape& shape, float zoom);

public:

    /** To obtain typical "2D pixel linear brightness values" use the
        default arguments for exposureConstant and imageGamma. If you want
        your colors on the range [0, 255] instead of [0, 2], set exposureConstant = 1.0f / 255.0f.
    */
    App(const std::string& windowCaption, int imageWidth, int imageHeight, float zoom = 2.0f,
        float exposureConstant = 1.0f, float imageGamma = 2.1f);

    virtual ~App() {}

    virtual void setPixel(int x, int y, const Color& c) = 0;

    virtual Color pixel(int x, int y) const = 0;

    /* Draws a 1-pixel thick line from (x0, y0) to (x1, y1) in color c. Pixels drawn outside
       of the image frame are ignored. */
    virtual void drawLine(int x0, int y0, int x1, int y1, const Color& c) = 0;
    
    /* Draws a 1-pixel thick box with corners at (x0, y0), (x1, y0),
       (x0, y1), and (x1, y1) in color c.  Pixels drawn outside of the
       image frame are ignored.  */
    virtual void drawBox(int x0, int y0, int x1, int y1, const Color& c) = 0;

    /** Finds all points f(x) = 0 for x in [xMin, xMax] and appends
        them to rootY. Assumes that f(x) is zero for finitely many
        values of x. Uses numerical methods and may miss roots. */
    virtual void findRoots(const Function& f, float xMin, float xMax, std::vector<float>& root) const {}

    /** Finds the smallest value of x on [xMin, xMax] for which f(x) =
        0, for a conservative distance estimator f(x) in which f(x0) is a conservative
        estimate of the distance along the x-axis from x0 to the root. i.e., all f(y) > 0
        on y = [x0, x0 + |f(x0)|] if f(x0) > 0.  If there is no root on the interval, returns nan. */
    virtual float findSmallestRootOfDistanceFunction(const Function& f, float xMin, float xMax) const { return NAN; }

    /** Saves the current image in PPM or TGA format. It must have a
        lower-case extension. */
    void saveImage(const std::string& filename);

    /** Call this to start the App executing. */
    void run();

    /** Called by App. Override with your image rendering code. */
    virtual void onGraphics() = 0;

    /** The default implementation quits the program when ESC is pressed */
    virtual void onKeyPress(unsigned char keyCode);

    /** Check m_mouseX and m_mouseY for the pixel coordinates.
        The button parameter is one of GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON. 
    */
    virtual void onMouseRelease(int button) {}

    /** \see onMouseRelease */
    virtual void onMousePress(int button) {}
};

#endif
