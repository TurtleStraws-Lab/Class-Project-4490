// program: UNCANNY VALLEY
// date: FALL 2026
// Framework for a 3D game.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include "defs.h"
#include "log.h"
#include "fonts.h"
#include <unistd.h>
#include <ctime>
#include "image.h"
#include "math_utils.h"
#include "camera.h"
#include "enemy.h"
#include "geometry.h"

Image scary_face("scary_face.png");

class Global {
public:
    int xres, yres;
    Flt aspectRatio;
    Camera camera;
    Enemy enemy;
    GLfloat lightPosition[4];
    int state;
    int start_screen;
    int menu_screen;
    time_t timeStart;
    time_t timeCurrent;
    int fps;

    const float HALLWAY_WIDTH  = 12.0f;
    const float HALLWAY_HEIGHT = 10.0f;
    const float HALLWAY_LENGTH = 500.0f;

    Global() {
        xres = 800;
        yres = 600;
        aspectRatio = (GLfloat)xres / (GLfloat)yres;
        VecMake(100.0f, 240.0f, 40.0f, lightPosition);
        lightPosition[3] = 1.0f;
        state = 1;
        fps = 0;
        start_screen = 1;
        menu_screen = 0;
    }

    void init_opengl();
    void init();
    void check_mouse(XEvent *e);
    int  check_keys(XEvent *e);
    void physics();
    void render();

    void clampPlayerToHallway(Camera &cam) {
        float margin = 0.5f;
        if (cam.position[0] < -HALLWAY_WIDTH + margin) cam.position[0] = -HALLWAY_WIDTH + margin;
        if (cam.position[0] >  HALLWAY_WIDTH - margin) cam.position[0] =  HALLWAY_WIDTH - margin;
        if (cam.position[1] < 1.0f)                    cam.position[1] = 1.0f;
        if (cam.position[1] > HALLWAY_HEIGHT - 1.0f)   cam.position[1] = HALLWAY_HEIGHT - 1.0f;
        if (cam.position[2] < 0.1f)                    cam.position[2] = 0.1f;
        if (cam.position[2] > HALLWAY_LENGTH - margin)  cam.position[2] = HALLWAY_LENGTH - margin;
    }
} g;

class X11_wrapper {
private:
    Display *dpy;
    Window win;
    GLXContext glc;
public:
    X11_wrapper() {
        Window root;
        GLint att[] = { GLX_RGBA,
                        GLX_STENCIL_SIZE, 2,
                        GLX_DEPTH_SIZE, 24,
                        GLX_DOUBLEBUFFER, None };
        Colormap cmap;
        XSetWindowAttributes swa;
        setup_screen_res(640, 480);
        dpy = XOpenDisplay(NULL);
        if (dpy == NULL) {
            printf("\n\tcannot connect to X server\n\n");
            exit(EXIT_FAILURE);
        }
        root = DefaultRootWindow(dpy);
        XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
        if (vi == NULL) {
            printf("\n\tno appropriate visual found\n\n");
            exit(EXIT_FAILURE);
        }
        cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
        swa.colormap = cmap;
        swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                         PointerMotionMask |
                         MotionNotify | ButtonPressMask | ButtonReleaseMask |
                         StructureNotifyMask | SubstructureNotifyMask;
        win = XCreateWindow(dpy, root, 0, 0, g.xres, g.yres, 0,
                            vi->depth, InputOutput, vi->visual,
                            CWColormap | CWEventMask, &swa);
        set_title();
        glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
        glXMakeCurrent(dpy, win, glc);
    }
    ~X11_wrapper() {
        XDestroyWindow(dpy, win);
        XCloseDisplay(dpy);
    }
    void setup_screen_res(const int w, const int h) {
        g.xres = w;
        g.yres = h;
        g.aspectRatio = (GLfloat)g.xres / (GLfloat)g.yres;
    }
    void check_resize(XEvent *e) {
        if (e->type != ConfigureNotify) return;
        XConfigureEvent xce = e->xconfigure;
        if (xce.width != g.xres || xce.height != g.yres)
            reshape_window(xce.width, xce.height);
    }
    void reshape_window(int width, int height) {
        setup_screen_res(width, height);
        glViewport(0, 0, (GLint)width, (GLint)height);
        glMatrixMode(GL_PROJECTION); glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
        glOrtho(0, g.xres, 0, g.yres, -1, 1);
        set_title();
    }
    void set_title() {
        XMapWindow(dpy, win);
        XStoreName(dpy, win, "UNCANNY VALLEY");
    }
    bool getXPending()  { return XPending(dpy); }
    XEvent getXNextEvent() {
        XEvent e;
        XNextEvent(dpy, &e);
        return e;
    }
    void swapBuffers() { glXSwapBuffers(dpy, win); }
} x11;

int main()
{
    g.init_opengl();
    scary_face.upload();
    srand(time(NULL));
    int done = 0;
    int nframes = 0;
    g.timeStart = time(nullptr);
    int starttime = time(NULL);

    while (!done) {
        while (x11.getXPending()) {
            XEvent e = x11.getXNextEvent();
            x11.check_resize(&e);
            g.check_mouse(&e);
            done = g.check_keys(&e);
        }
        g.physics();
        g.render();
        ++nframes;
        int currtime = time(NULL);
        if (currtime > starttime) {
            starttime = currtime;
            g.fps = nframes;
            nframes = 0;
        }
        x11.swapBuffers();
    }
    cleanup_fonts();
    return 0;
}

void Global::init()
{
    // Place general program initializations here.
}

void Global::init_opengl()
{
    glClearColor(0.0f, 0.1f, 0.3f, 0.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_POSITION, g.lightPosition);
    glEnable(GL_LIGHT0);
    GLfloat l1ambient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat l1diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat l1specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT1, GL_AMBIENT,  l1ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  l1diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, l1specular);
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();
}

void Global::check_mouse(XEvent *e)
{
    static int savex = 0;
    static int savey = 0;
    if (e->type == ButtonRelease) return;
    if (e->type == ButtonPress) {
        if (e->xbutton.button == 1) {
            g.camera.position[0] += g.camera.direction[0];
            g.camera.position[1] += g.camera.direction[1];
            g.camera.position[2] += g.camera.direction[2];
            if (g.state == 1) g.camera.force[1] += 0.09;
        }
        if (e->xbutton.button == 3) {
            g.camera.position[0] -= g.camera.direction[0];
            g.camera.position[1] -= g.camera.direction[1];
            g.camera.position[2] -= g.camera.direction[2];
        }
    }
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        if (e->xbutton.x - savex < 0) g.camera.lookLeftRight(0.01);
        else                           g.camera.lookLeftRight(-0.01);
        if (e->xbutton.y - savey < 0) g.camera.lookUpDown(-0.01);
        else                           g.camera.lookUpDown(0.01);
        savex = e->xbutton.x;
        savey = e->xbutton.y;
    }
}

int Global::check_keys(XEvent *e)
{
    static bool keys[65536] = {};

    if (e->type == KeyPress) {
        int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
        keys[key] = true;
        switch (key) {
            case XK_4:     g.start_screen = 0; g.menu_screen = 1; break;
            case XK_5:     g.menu_screen = 0; break;
            case XK_space:
                if (g.state == 1 && g.camera.position[1] <= 3.2)
                    g.camera.force[1] += 0.2;
                break;
            case XK_Escape: return 1;
        }
    }
    if (e->type == KeyRelease) {
        int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
        keys[key] = false;
    }

    bool moved = false;

    if (keys[XK_w]) {
        g.camera.last_position[0] = g.camera.position[0];
        g.camera.last_position[1] = g.camera.position[1];
        g.camera.last_position[2] = g.camera.position[2];
        g.camera.position[0] += g.camera.direction[0];
        g.camera.position[1] += g.camera.direction[1];
        g.camera.position[2] += g.camera.direction[2];
        if (g.state == 1) g.camera.force[1] += 0.09;
        moved = true;
    }
    if (keys[XK_s]) {
        g.camera.last_position[0] = g.camera.position[0];
        g.camera.last_position[1] = g.camera.position[1];
        g.camera.last_position[2] = g.camera.position[2];
        g.camera.position[0] -= g.camera.direction[0];
        g.camera.position[1] -= g.camera.direction[1];
        g.camera.position[2] -= g.camera.direction[2];
        moved = true;
    }

    if (keys[XK_d]) { g.camera.moveLeftRight( 1.0); moved = true; }
    if (keys[XK_a]) { g.camera.moveLeftRight(-1.0); moved = true; }

    if (keys[XK_u])       g.camera.translate(0.0, 0.2, 0.0);
    if (keys[XK_Shift_L]) g.camera.position[1] -= 0.2;

    if (keys[XK_Down])  g.camera.lookUpDown(0.1);
    if (keys[XK_Up])    g.camera.lookUpDown(-0.1);
    if (keys[XK_Left])  g.camera.lookLeftRight(0.1);
    if (keys[XK_Right]) g.camera.lookLeftRight(-0.1);

    if (moved) clampPlayerToHallway(g.camera);

    return 0;
}

const Flt GRAVITY = -0.02f;

void Global::physics()
{
    if (g.state == 1) {
        g.camera.vel[1] += GRAVITY;
        g.camera.vel[1] += g.camera.force[1];
        g.camera.force[1] = 0.0;
        g.camera.position[1] += g.camera.vel[1];
        if (g.camera.position[1] < 3.0) {
            g.camera.position[1] = 3.0;
            g.camera.vel[1] = 0.0;
        }
    }
    enemy.update(camera);
}

void Global::render()
{
    g.timeCurrent = time(nullptr);
    double elapsed = difftime(timeCurrent, timeStart);

    if (g.start_screen && (elapsed < 9.0)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, g.xres, g.yres);
        glMatrixMode(GL_PROJECTION); glLoadIdentity();
        gluOrtho2D(0, g.xres, 0, g.yres);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);

        if (elapsed > 5.0) {
            float t = (float)(elapsed - 5.0) / 4.0f;
            if (t > 1.0f) t = 1.0f;
            float maxW = 300.0f, maxH = 300.0f;
            float w = 10.0f + (maxW - 10.0f) * t;
            float h = 10.0f + (maxH - 10.0f) * t;
            float cx = g.xres / 2.0f, cy = g.yres / 2.0f;
            float x0 = cx - w*0.5f, x1 = cx + w*0.5f;
            float y0 = cy - h*0.5f, y1 = cy + h*0.5f;
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, scary_face.texid);
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_QUADS);
                glTexCoord2f(0,1); glVertex2f(x0,y0);
                glTexCoord2f(1,1); glVertex2f(x1,y0);
                glTexCoord2f(1,0); glVertex2f(x1,y1);
                glTexCoord2f(0,0); glVertex2f(x0,y1);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }

        Rect r;
        r.bot = g.yres - 50; r.left = g.xres / 2; r.center = 20;
        ggprint16(&r, 16, 0x00ffffff, "UNCANNY VALLEY");
        Rect r2;
        r2.bot = (int)(g.yres * 0.15f); r2.left = g.xres / 2; r2.center = 20;
        ggprint12(&r2, 16, 0x00ffffff, "Press 4 to Skip");
        glPopAttrib();

    } else if (g.start_screen) {
        g.start_screen = 0;
        g.menu_screen  = 1;

    } else if (g.menu_screen) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, g.xres, g.yres);
        glMatrixMode(GL_PROJECTION); glLoadIdentity();
        gluOrtho2D(0, g.xres, 0, g.yres);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);

        int bw = 200, bh = 60;
        int cx = g.xres / 2, cy = g.yres / 2;
        glDisable(GL_DEPTH_TEST);

        glColor4f(0.1f, 0.1f, 0.1f, 0.85f);
        glBegin(GL_QUADS);
            glVertex2f(cx-bw/2, cy-bh/2); glVertex2f(cx+bw/2, cy-bh/2);
            glVertex2f(cx+bw/2, cy+bh/2); glVertex2f(cx-bw/2, cy+bh/2);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(cx-bw/2, cy-bh/2); glVertex2f(cx+bw/2, cy-bh/2);
            glVertex2f(cx+bw/2, cy+bh/2); glVertex2f(cx-bw/2, cy+bh/2);
        glEnd();

        Rect r;
        r.bot = g.yres - 50; r.left = g.xres / 2; r.center = 20;
        ggprint16(&r, 16, 0x00ffffff, "UNCANNY VALLEY");
        Rect r2;
        r2.bot = cy; r2.left = cx - 50; r2.center = 0;
        ggprint12(&r2, 16, 0x00ffffff, "Press 5 to Play");

        glEnable(GL_DEPTH_TEST);
        glPopAttrib();

    } else {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION); glLoadIdentity();
        gluPerspective(45.0f, g.aspectRatio, 0.5f, 1000.0f);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();

        Vec up = {0.0, 1.0, 0.0};
        gluLookAt(
            g.camera.position[0],  g.camera.position[1],  g.camera.position[2],
            g.camera.position[0]+g.camera.direction[0],
            g.camera.position[1]+g.camera.direction[1],
            g.camera.position[2]+g.camera.direction[2],
            up[0], up[1], up[2]);

        glLightfv(GL_LIGHT0, GL_POSITION, g.lightPosition);

        glPushMatrix();
        glLoadIdentity();
        GLfloat flashlightPos_eye[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        GLfloat flashlightDir_eye[3] = {0.0f, 0.0f, -1.0f};
        glLightfv(GL_LIGHT1, GL_POSITION,       flashlightPos_eye);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION,  flashlightDir_eye);
        glLightf(GL_LIGHT1,  GL_SPOT_CUTOFF,    25.0f);
        glLightf(GL_LIGHT1,  GL_SPOT_EXPONENT,  50.0f);
        glLightf(GL_LIGHT1,  GL_CONSTANT_ATTENUATION,   1.0f);
        glLightf(GL_LIGHT1,  GL_LINEAR_ATTENUATION,     0.01f);
        glLightf(GL_LIGHT1,  GL_QUADRATIC_ATTENUATION,  0.001f);
        glEnable(GL_LIGHT1);
        glPopMatrix();

        drawHallway();
        enemy.draw();

        // HUD (2D overlay)
        glViewport(0, 0, g.xres, g.yres);
        glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
        glMatrixMode(GL_PROJECTION); glLoadIdentity();
        gluOrtho2D(0, g.xres, 0, g.yres);
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);

        Rect r;
        r.bot = g.yres - 20; r.left = 10; r.center = 0;
        ggprint8b(&r, 16, 0x00887766, "UNCANNY VALLEY");
        ggprint8b(&r, 16, 0x00ff00ff, "use WASD to MOVE");
        ggprint8b(&r, 16, 0x00ff00ff, "use SPACE to JUMP");
        ggprint8b(&r, 16, 0x00ff00ff, "use ARROW KEYS to LOOK AROUND");
        ggprint8b(&r, 16, 0x0000ff00, "FPS: %i", g.fps);
        glPopAttrib();
    }
}
