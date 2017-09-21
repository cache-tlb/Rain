#include <cstdio>
#include "CanvasWrapper.h"
#include "App.h"

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLMesh.h"
#include "GLTexture.h"
#include "Shader.h"
#include "VSMathLib.h"

std::shared_ptr<App> app;
void resize(int w, int h) {
    app->resize(w, h);
}

void draw() {
    app->render();
}

void processKeys(unsigned char key, int xx, int yy) {	
    app->processKeys(key, xx, yy);
}

void processMouseButtons(int button, int state, int xx, int yy) {
    app->processMouseButtons(button, state, xx, yy);
}

void processMouseMotion(int xx, int yy) {
    app->processMouseMotion(xx, yy);
}

void processMouseWheel(int button, int dir, int x, int y) {
    app->processMouseWheel(button, dir, x, y);
}

void idle() {
    app->idle();
}

void initOpenGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // this line cause glGetError
    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);

    glEnable(GL_POINT_SMOOTH); // this line cause glGetError
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);           // polygonj smooth should be turned off
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glGetError();
}

void initApp(int w, int h) {
    app = std::shared_ptr<App>(new App(w, h));
    app->init();
}

int main(int argc, char **argv) {
/*    App app(1024, 768);
    app.init();
    while (true) {
        app.draw();
    }
    */

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    //glutInitContextFlags(GLUT_DEBUG);

    int w = 1600, h = 900;
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(w, h);
    glutCreateWindow("Rain");

    glutDisplayFunc(draw);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);

    glutKeyboardFunc(processKeys);
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);
    glutMouseWheelFunc(processMouseWheel);

    //	return from main loop
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    //	Init GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();

    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version: %s\n", glGetString(GL_VERSION));
    printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    initOpenGL();

    initApp(w, h);
    glutMainLoop();

    return 0;
}

