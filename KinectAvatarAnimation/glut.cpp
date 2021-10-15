#include "glut.h"
#include "main.h"

/*
* @function calls routines for refreshing and
*           drawing on glut window generated
*/
void draw() {
    drawKinectData();
    glutSwapBuffers();
}

/*
* @function calls function handler for
*           clicking left button on mouse
*           glutMouseFunc is actived from window-inside clicks only
* @params button can be GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON or GLUT_MIDDLE_BUTTON
*         state can be GLUT_DOWN or GLUT_UP
*         x is absissa coordinate in window
*         y is ordinate coordinate in window
* @alert params are generated automatically
*/
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON)
        mouseManager(state, x, y);
}

/*
* @function calls function handler for
*           moving mouse on window
* @params x is absissa coordinate in window
*         y is ordinate coordinate in window
*/
void movingMouse(int x, int y) {
    movingMouseManager(x, y);
}

/*
* @function setup draw function and other
*           handler functions for input events:
*               EVENT              FUNCTION
*           continuously      ---> draw
*           mouse clicking    ---> mouse
*           mouse moving      ---> movingMouse
*/
void execute() {
    glutDisplayFunc(draw);
    glutIdleFunc(draw);
    glutMouseFunc(mouse);
    glutMotionFunc(movingMouse);
    glutMainLoop();
}

/*
* @function initialize a window enviroment with glut functions
*           and openGL enviroment with glew function
* @params argc is main arg UNMODIFIED
*         argv is main arg UNMODIFIED
*/
void init(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    /** GLUT_DOUBLE creates two framebuffer : 
    *   One is used for drawing, the other one for displaying.
    *   At the end of each frame, these buffers are swapped. 
    *   Doing so, the view is only changed at once when a frame is finished 
    *   and all objects are visible for the same time.
    */
    glutInitWindowSize(width, height);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Kinect Avatar Animation");

    glewInit();
}