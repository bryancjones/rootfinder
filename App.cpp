// Code taken from http://cs.williams.edu/~morgan/cs136/schedule.html

#include <GL/glut.h>
#include <cassert>
#include <stdio.h>
#include "App.h"

const float App::minimumDistanceToSurface = 0.0003f;

App* App::instance = NULL;
static const float deviceGamma = 2.1f;
static const float fps = 30.0f;


App::App(const std::string& windowCaption, int imageWidth, int imageHeight, float zoom, float exposureConstant, float imageGamma) : 
    m_windowCaption(windowCaption), m_zoom(zoom), m_exposureConstant(exposureConstant), m_imageGamma(imageGamma),
    m_frameTimeMilliseconds(int(1000.0f / fps + 0.5f)),
    m_imageWidth(imageWidth), m_imageHeight(imageHeight) {

    assert((imageWidth > 0) && (imageHeight > 0));
    assert(zoom > 0);
    m_imageData.resize(m_imageWidth * m_imageHeight);
    instance = this;


    //************* CHANGED THESE VALUES TO PLAY WITH COLORS ***********************
    //m_lightDirection = normalize(Vector3(1, 1, -1));
    m_lightDirection = normalize(Vector3(1, 1, -1));

    //m_keyLightColor  = Color(1.0f, 1.0f, 1.0f);
    m_keyLightColor = Color(0.0f, 0.0f, 1.0f);
    //m_fillLightColor = Color(0.0f, 0.2f, 0.7f);
    m_fillLightColor = Color(1.0f, 0.0f, 0.0f);

    //m_backgroundGradientCenterColor = Color(0.9f, 0.9f, 0.9f);
    m_backgroundGradientCenterColor = Color(1.0f, 0.0f, 0.0f);
    //m_backgroundGradientRimColor    = Color(0.5f, 0.3f, 0.1f);
    m_backgroundGradientRimColor = Color(0.1f, 0.0f, 0.0f);
    //*****************************************************************************
}


void App::run() {
    int argc = 0;
    
    // See glut specs at https://www.opengl.org/resources/libraries/glut/spec3/spec3.html

    // Initialize OpenGL
    glutInit(&argc, NULL);
    glutInitWindowSize(int(ceil(float(m_imageWidth) * m_zoom)), int(ceil(float(m_imageHeight) * m_zoom)));
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow(m_windowCaption.c_str());
    glutKeyboardFunc(&staticOnKeyPress);
    glutDisplayFunc(&staticOnGraphics);
    glutMotionFunc(&staticOnMouseMotion);
    glutPassiveMotionFunc(&staticOnMouseMotion);
    glutReshapeFunc(&staticReshape);

    // Set the color scale applied as textures are uploaded to be the exposure constant
    glMatrixMode(GL_COLOR);
    glLoadIdentity();
    glScalef(m_exposureConstant, m_exposureConstant, m_exposureConstant);

    // Create a gamma correction color table for texture load
    if (m_imageGamma != deviceGamma) {
        const float gamma = deviceGamma / m_imageGamma;
        std::vector<Color> gammaTable(256);
        for (unsigned int i = 0; i < gammaTable.size(); ++i) {
            gammaTable[i] = pow(Color::white() * i / (gammaTable.size() - 1.0f), 1.0f / gamma);
        }
        glColorTable(GL_POST_COLOR_MATRIX_COLOR_TABLE, GL_RGB, gammaTable.size(), GL_RGB, GL_FLOAT, &gammaTable[0]);
        glEnable(GL_POST_COLOR_MATRIX_COLOR_TABLE);
    }
    
    // Create a texture, upload our image, and bind it (assume a
    // version of GL that supports NPOT textures)
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_imageWidth, m_imageHeight, 0, GL_RGB, GL_FLOAT, &m_imageData[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glEnable(GL_TEXTURE_2D);

    // The vertices of a 2D quad mesh containing a single CCW square
    static const float corner[] = {0,0, 0,1, 1,1, 1,0};

    // Bind the quad mesh as the active geometry
    glVertexPointer(2, GL_FLOAT, 0, corner);
    glTexCoordPointer(2, GL_FLOAT, 0, corner);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    // Set orthographic projection that stretches the unit square to the
    // dimensions of the image
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 1, 0, 0, 2);

    // Kick off the timer
    timerCallback(0);
    glutMainLoop();
}


void App::timerCallback(int value) {
    // Request animation
    glutPostRedisplay();
    glutTimerFunc(instance->m_frameTimeMilliseconds, &timerCallback, value);
}


void App::staticOnKeyPress(unsigned char key, int x, int y) {
    instance->m_mouseX = x;
    instance->m_mouseY = y;
    instance->onKeyPress(key);
}


void App::staticOnMousePress(int button, int state, int x, int y) {
    instance->m_mouseX = x;
    instance->m_mouseY = y;
    if (state == GLUT_DOWN) {
        instance->onMousePress(button);
    } else {
        instance->onMouseRelease(button);
    }
}


void App::staticOnMouseMotion(int x, int y) {
    instance->m_mouseX = x;
    instance->m_mouseY = y;
}


void App::staticReshape(int width, int height) {
    // Restore the size originally requested
    glutReshapeWindow(int(ceil(float(instance->m_imageWidth)  * instance->m_zoom)), 
                      int(ceil(float(instance->m_imageHeight) * instance->m_zoom)));
}


void App::onKeyPress(unsigned char key) {
    if (key == 27) { ::exit(0); }
}


void App::staticOnGraphics() {
    instance->onGraphics();

    // Upload the image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, instance->m_imageWidth, instance->m_imageHeight, 0,
                 GL_RGB, GL_FLOAT, &instance->m_imageData[0]);

    // Draw a full-screen quad of the image
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_QUADS, 0, 4);
    glutSwapBuffers();
}


void App::saveImage(const std::string& filename) {
    assert(filename.size() > 4);
    if (filename.substr(filename.length() - 4) == ".tga") {
        saveTGA(filename);
    } else if (filename.substr(filename.length() - 4) == ".ppm") {
        savePPM(filename);
    } else {
        // Bad file format
        assert(false);
    }
}


static int PPMGammaCorrect(float v, float e, float g) {
    return int(pow(std::min(1.0f, std::max(0.0f, v * e)), 1.0f / g) * 255.0f);
}


void App::savePPM(const std::string& filename) const {
    FILE* file = fopen(filename.c_str(), "wt");
    fprintf(file, "P3 %d %d 255\n", m_imageWidth, m_imageHeight); 

    const float gamma = deviceGamma / m_imageGamma;

    for (int y = 0, i = 0; y < m_imageHeight; ++y) {
        fprintf(file, "\n# y = %d\n", y);                                                               
        for (int x = 0; x < m_imageWidth; ++x, ++i) {
            const Color& c = m_imageData[i];
            fprintf(file, "%d %d %d\n", 
                    PPMGammaCorrect(c.r, m_exposureConstant, gamma), 
                    PPMGammaCorrect(c.g, m_exposureConstant, gamma),
                    PPMGammaCorrect(c.b, m_exposureConstant, gamma));
        }
    }
    fclose(file);
}


void App::saveTGA(const std::string& filename) const {
    // http://www.paulbourke.net/dataformats/tga/
    FILE* file = fopen(filename.c_str(), "wb");

    // Header
    putc(0, file);
    putc(0, file);
    putc(2, file);                         /* uncompressed RGB */
    putc(0, file); putc(0, file);
    putc(0, file); putc(0, file);
    putc(0, file);
    putc(0, file); putc(0, file);           /* X origin */
    putc(0, file); putc(0, file);           /* y origin */
    printf("%d\n", m_imageWidth); 
    putc((m_imageWidth & 0x00FF), file);
    putc((m_imageWidth & 0xFF00) >> 8, file);

    putc((m_imageHeight & 0x00FF), file);
    putc((m_imageHeight & 0xFF00) >> 8, file);

    putc(24, file);                        /* 32 bit bitmap */
    putc( (1 << 5), file);                 /* origin = top left */

    const float gamma = deviceGamma / m_imageGamma;

    for (int y = 0, i = 0; y < m_imageHeight; ++y) {
        for (int x = 0; x < m_imageWidth; ++x, ++i) {
            const Color& c = m_imageData[i];
            putc(PPMGammaCorrect(c.b, m_exposureConstant, gamma), file);
            putc(PPMGammaCorrect(c.g, m_exposureConstant, gamma), file);
            putc(PPMGammaCorrect(c.r, m_exposureConstant, gamma), file);
            //putc(0xFF, file); // alpha = 1
        }
    }
    
    fclose(file);
}

////////////////////////////////////////////////////////////

float Shape::distance(const Point3& point) const {
    float d, ignore;
    getDistanceAndShade(point, d, ignore);
    return d;
}


float Shape::shade(const Point3& point) const {
    float ignore, s;
    getDistanceAndShade(point, ignore, s);
    return s;
}


void Shape::setRotation(float yaw, float pitch, float roll) {
    rotation = 
        Matrix3x3(cos(roll),  -sin(roll),       0.0f, 
                  sin(roll),  cos(roll), 0.0f,
                  0.0f, 0.0f, 1.0f) *

        Matrix3x3(1.0f,  0.0f,       0.0f, 
                  0.0f,  cos(pitch), sin(pitch),
                  0.0f, -sin(pitch), cos(pitch)) *
        
        Matrix3x3(cos(yaw), 0.0f, -sin(yaw), 
                  0.0f,     1.0f,  0.0f, 
                  sin(yaw), 0.0f,  cos(yaw));
}

/////////////////////////////////////////////////////////////

Color App::computeRayCastSample(const Vector2 coord, const Shape& shape, float zoom) {
    // A small step, used for computing the surface normal
    // by numerical differentiation. A scaled up version of
    // this is also used for computing a low-frequency gradient.
    const float epsilon = minimumDistanceToSurface * 5.0f;
    
    const float cameraDistance = 5.0f;
    Point3 rayOrigin = Point3(2.0f * coord.x / float(m_imageWidth) - 1.0f, 1.0f - 2.0f * coord.y / float(m_imageHeight), -cameraDistance);

    // Correct for aspect ratio
    rayOrigin.x *= float(m_imageWidth) / float(m_imageHeight);
    
    const Vector3& rayDirection = normalize(normalize(Point3(0.0f, 0.0f, 1.0f) - rayOrigin) + 
                                            0.2f * Point3(rayOrigin.x, rayOrigin.y, 0.0f) / zoom);

    
    // Point on (technically, near) the surface of the shape
    Point3 X;

    const float t = findSmallestRootOfDistanceFunction(DistanceToShapeOnRay(rayOrigin, rayDirection, shape), 0.0f, 10.0f);
    const bool hit = ! isnan(t);
    X = rayOrigin + t * rayDirection;

    Color color;
    if (hit) {
        // Compute AO term
        float d, AO;
        shape.getDistanceAndShade(X, d, AO);
        
        // Back away from the surface a bit before computing the gradient
        X = X - rayDirection * epsilon;
        
        // Accurate micro-normal by numerical derivative
        Vector3 n = normalize(Vector3(d - shape.distance(X - Vector3(epsilon, 0.0f, 0.0f)),
                                      d - shape.distance(X - Vector3(0.0f, epsilon, 0.0f)),
                                      d - shape.distance(X - Vector3(0.0f, 0.0f, epsilon))));
        
        // Broad-scale normal to large shape
        const Vector3& n2 = normalize(Vector3(d - shape.distance(X - Vector3(epsilon * 50.0f, 0.0f, 0.0f)),
                                              d - shape.distance(X - Vector3(0.0f, epsilon * 50.0f, 0.0f)),
                                              d - shape.distance(X - Vector3(0.0f, 0.0f, epsilon * 50.0f))));

        // Bend the local surface normal by the
        // gross local shape normal and the bounding sphere
        // normal to avoid the hyper-detailed look
        n = normalize(n + n2 + normalize(X));

        // Fade between the key and fill light based on the normal (Gooch-style wrap shading).
        // Also darken the surface in cracks (on top of the AO term)
        return AO * (mix(m_fillLightColor, m_keyLightColor, AO * clamp(0.7f * dot(m_lightDirection, n) + 0.6f, 0.0f, 1.0f)) * surfaceColor(AO) +
                      // Give the feel of blowing out the highlights with a yellow tint
                      pow(max(dot(m_lightDirection, n2), 0.0f), 5.0f) * Color(1.3f, 1.2f, 0.0f));
    } else {
        // No hit: return the background gradient
        return mix(m_backgroundGradientCenterColor, m_backgroundGradientRimColor, 
                   sqrt(length((coord / Vector2(m_imageWidth, m_imageHeight) - Vector2(0.66f, 0.66f)) * 2.5f)));
    }
}


Color App::computeRayCastPixel(const Point2 coord, const Shape& function, float zoom) {
    // 4x rotated-grid SSAA for antialiasing
    Color color = 
        //computeRayCastSample(coord, function, zoom);
        (computeRayCastSample(coord + Vector2(-0.125f, -0.375f), function, zoom) + 
         computeRayCastSample(coord + Vector2(+0.375f, -0.125f), function, zoom) + 
         computeRayCastSample(coord + Vector2(+0.125f, +0.375f), function, zoom) +
         computeRayCastSample(coord + Vector2(-0.375f, +0.125f), function, zoom)) / 4.0f;

    
    // Coarse RGB->sRGB encoding via sqrt
    color = sqrt(color);
    
    // Vignetting (from iq https://www.shadertoy.com/view/MdX3Rr)
    const Vector2& xy = 2.0f * coord / Vector2(m_imageWidth, m_imageHeight) - Vector2(1.0f, 1.0f);
    return color * (0.5f + 0.5f * pow((xy.x + 1.0f) * (xy.y + 1.0f) * (xy.x - 1.0f) * (xy.y - 1.0f), 0.2f));
}


void App::drawRayCastImage(const Shape& shape, float zoom) {
    for (int y = 0; y < m_imageHeight; ++y) {
        for (int x = 0; x < m_imageWidth; ++x) {
            setPixel(x, y, computeRayCastPixel(Point2(float(x) + 0.5f, float(y) + 0.5f), shape, zoom));
        } // x
    } // y
}
