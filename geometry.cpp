#include "geometry.h"
#include <GL/gl.h>
#include <math.h>

void cube(float w1, float h1, float d1)
{
    float w = w1 * 0.5;
    float d = d1 * 0.5;
    float h = h1 * 0.5;
    glBegin(GL_QUADS);
        // top
        glNormal3f( 0.0f, 1.0f, 0.0f);
        glVertex3f( w, h,-d); glVertex3f(-w, h,-d);
        glVertex3f(-w, h, d); glVertex3f( w, h, d);
        // bottom
        glNormal3f( 0.0f,-1.0f, 0.0f);
        glVertex3f( w,-h, d); glVertex3f(-w,-h, d);
        glVertex3f(-w,-h,-d); glVertex3f( w,-h,-d);
        // front
        glNormal3f( 0.0f, 0.0f, 1.0f);
        glVertex3f( w, h, d); glVertex3f(-w, h, d);
        glVertex3f(-w,-h, d); glVertex3f( w,-h, d);
        // back
        glNormal3f( 0.0f, 0.0f,-1.0f);
        glVertex3f( w,-h,-d); glVertex3f(-w,-h,-d);
        glVertex3f(-w, h,-d); glVertex3f( w, h,-d);
        // left
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-w, h, d); glVertex3f(-w, h,-d);
        glVertex3f(-w,-h,-d); glVertex3f(-w,-h, d);
        // right
        glNormal3f( 1.0f, 0.0f, 0.0f);
        glVertex3f( w, h,-d); glVertex3f( w, h, d);
        glVertex3f( w,-h, d); glVertex3f( w,-h,-d);
    glEnd();
}

void tube(int n, float rad, float len)
{
    const int MAX_POINTS = 100;
    static float pts[MAX_POINTS][3];
    static int firsttime = 1;
    if (firsttime) {
        firsttime = 0;
        double angle = 0.0;
        double inc = (3.1415926535 * 2.0) / (double)n;
        for (int i = 0; i < n; i++) {
            pts[i][0] = cos(angle) * rad;
            pts[i][2] = sin(angle) * rad;
            pts[i][1] = 0.0f;
            angle += inc;
        }
    }
    glBegin(GL_QUADS);
        for (int i = 0; i < n; i++) {
            int j = (i+1) % n;
            glNormal3f(pts[i][0], 0.0, pts[i][2]);
            glVertex3f(pts[i][0], 0.0, pts[i][2]);
            glVertex3f(pts[i][0], len, pts[i][2]);
            glNormal3f(pts[j][0], 0.0, pts[j][2]);
            glVertex3f(pts[j][0], len, pts[j][2]);
            glVertex3f(pts[j][0], 0.0, pts[j][2]);
        }
    glEnd();
}

void drawHallway()
{
    float width  = 12.0f;
    float height = 10.0f;
    float length = 500.0f;

    int tilesX = 10, tilesY = 10, tilesZ = 50;
    float tileW = (2 * width)  / tilesX;
    float tileH = height       / tilesY;
    float tileL = length       / tilesZ;

    // Floor
    for (int i = 0; i < tilesX; i++) {
        for (int j = 0; j < tilesZ; j++) {
            bool even = (i + j) % 2 == 0;
            glColor3ub(even ? 60 : 100, even ? 60 : 100, even ? 60 : 100);
            float x0 = -width + i*tileW, x1 = x0+tileW;
            float z0 = j*tileL,          z1 = z0+tileL;
            glBegin(GL_QUADS);
                glNormal3f(0,1,0);
                glVertex3f(x0,0,z0); glVertex3f(x1,0,z0);
                glVertex3f(x1,0,z1); glVertex3f(x0,0,z1);
            glEnd();
        }
    }

    // Ceiling
    for (int i = 0; i < tilesX; i++) {
        for (int j = 0; j < tilesZ; j++) {
            bool even = (i + j) % 2 == 0;
            glColor3ub(even ? 40 : 80, even ? 40 : 80, even ? 40 : 80);
            float x0 = -width + i*tileW, x1 = x0+tileW;
            float z0 = j*tileL,          z1 = z0+tileL;
            glBegin(GL_QUADS);
                glNormal3f(0,-1,0);
                glVertex3f(x0,height,z0); glVertex3f(x1,height,z0);
                glVertex3f(x1,height,z1); glVertex3f(x0,height,z1);
            glEnd();
        }
    }

    // Left wall
    for (int j = 0; j < tilesZ; j++) {
        for (int i = 0; i < tilesY; i++) {
            bool even = (i + j) % 2 == 0;
            glColor3ub(even ? 60 : 100, even ? 60 : 100, even ? 60 : 100);
            float y0 = i*tileH, y1 = y0+tileH;
            float z0 = j*tileL, z1 = z0+tileL;
            glBegin(GL_QUADS);
                glNormal3f(1,0,0);
                glVertex3f(-width,y0,z0); glVertex3f(-width,y0,z1);
                glVertex3f(-width,y1,z1); glVertex3f(-width,y1,z0);
            glEnd();
        }
    }

    // Right wall
    for (int j = 0; j < tilesZ; j++) {
        for (int i = 0; i < tilesY; i++) {
            bool even = (i + j) % 2 == 0;
            glColor3ub(even ? 60 : 100, even ? 60 : 100, even ? 60 : 100);
            float y0 = i*tileH, y1 = y0+tileH;
            float z0 = j*tileL, z1 = z0+tileL;
            glBegin(GL_QUADS);
                glNormal3f(-1,0,0);
                glVertex3f(width,y0,z0); glVertex3f(width,y0,z1);
                glVertex3f(width,y1,z1); glVertex3f(width,y1,z0);
            glEnd();
        }
    }

    // Back wall (z=0)
    for (int i = 0; i < tilesX; i++) {
        for (int j = 0; j < tilesY; j++) {
            bool even = (i + j) % 2 == 0;
            glColor3ub(even ? 80 : 120, even ? 80 : 120, even ? 80 : 120);
            float x0 = -width + i*tileW, x1 = x0+tileW;
            float y0 = j*tileH,          y1 = y0+tileH;
            glBegin(GL_QUADS);
                glNormal3f(0,0,1);
                glVertex3f(x0,y0,0); glVertex3f(x1,y0,0);
                glVertex3f(x1,y1,0); glVertex3f(x0,y1,0);
            glEnd();
        }
    }

    // Front wall (z=length)
    for (int i = 0; i < tilesX; i++) {
        for (int j = 0; j < tilesY; j++) {
            bool even = (i + j) % 2 == 0;
            glColor3ub(even ? 80 : 120, even ? 80 : 120, even ? 80 : 120);
            float x0 = -width + i*tileW, x1 = x0+tileW;
            float y0 = j*tileH,          y1 = y0+tileH;
            glBegin(GL_QUADS);
                glNormal3f(0,0,-1);
                glVertex3f(x0,y0,length); glVertex3f(x1,y0,length);
                glVertex3f(x1,y1,length); glVertex3f(x0,y1,length);
            glEnd();
        }
    }
}
