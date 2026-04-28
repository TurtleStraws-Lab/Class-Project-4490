#ifndef ENEMY_H
#define ENEMY_H

#include "math_utils.h"
#include "camera.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdlib.h>
#include <ctime>

void cube(float w1, float h1, float d1);

class Enemy {
public:
    Vec pos;
    float yaw;
    bool eyesOn;
    int blinkTimer;

    Enemy() {
        VecMake(20.0f, 0.0f, -50.0f, pos);
        yaw = 0.0f;
        eyesOn = true;
        blinkTimer = 0;
    }

    void update(Camera &cam) {
        float dx = cam.position[0] - pos[0];
        float dz = cam.position[2] - pos[2];
        yaw = atan2f(dx, dz) * 180.0f / (float)M_PI;
        float dist = sqrtf(dx*dx + dz*dz);
        if (dist > 16.0f) {
            pos[0] += dx * 0.01f;
            pos[2] += dz * 0.01f;
        }
        blinkTimer++;
        if (blinkTimer > 300 + rand() % 300) {
            eyesOn = !eyesOn;
            blinkTimer = 0;
        }
    }

    void draw() {
        glPushMatrix();
        glTranslatef(pos[0], pos[1], pos[2]);
        glRotatef(yaw, 0.0f, 1.0f, 0.0f);

        float t = (float)clock() * 0.0006f;
        float stretchY = 1.0f + sinf(t) * 0.15f;
        float squashX  = 1.0f - sinf(t) * 0.08f;
        float squashZ  = 1.0f + cosf(t * 0.8f) * 0.05f;
        glRotatef(sinf(t * 1.2f) * 2.5f, 1, 0, 0);
        glRotatef(cosf(t * 1.0f) * 2.0f, 0, 0, 1);
        glScalef(squashX, stretchY, squashZ);

        // Body
        glColor3ub(90, 90, 90);
        glPushMatrix();
        float sway = sinf(t * 3.0f) * 0.3f;
        glTranslatef(sway, 2.8f, 0.0f);
        cube(1.0f, 4.5f, 0.8f);
        glPopMatrix();

        // Head
        glColor3ub(110, 110, 110);
        glPushMatrix();
        float jitterX = sinf(t * 3.0f) * 0.03f;
        float jitterY = cosf(t * 2.5f) * 0.03f;
        glTranslatef(jitterX, 5.2f + jitterY, 0.0f);
        glRotatef(sinf(t * 5.0f) * 10.0f, 0, 1, 0);
        cube(0.9f, 0.9f, 0.9f);
        glPopMatrix();

        // Eyes
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
        glDisable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
        if (eyesOn) glColor3ub(255, 0, 0);
        else        glColor3ub(0, 0, 0);

        // left eye
        glPushMatrix();
        glTranslatef(-0.25f, 4.6f, 0.8f);
        GLUquadric *q = gluNewQuadric();
        gluSphere(q, 0.15, 12, 12);
        gluDeleteQuadric(q);
        glPopMatrix();

        // right eye
        glPushMatrix();
        glTranslatef(0.25f, 4.6f, 0.8f);
        q = gluNewQuadric();
        gluSphere(q, 0.15, 12, 12);
        gluDeleteQuadric(q);
        glPopMatrix();

        glPopAttrib();
        glPopMatrix();
    }
};

#endif // ENEMY_H
