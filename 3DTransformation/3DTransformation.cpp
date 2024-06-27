#include <stdio.h>
#include <iostream>
#include <GL/freeglut.h>
#include <math.h>
using namespace std;

GLfloat Vertex[50][3]; //배열에 정점좌표(x좌표, y좌표)를 저장
GLfloat VertexBack[50][3]; // z축 정점좌표
GLint NumVertex = 0; //배열의 개수로 사용할 변수
GLint AddInputMode = 1; //1: Input, 2: Translation, 3: Rotation, 4:Scaling
GLfloat mx, my; //마우스누른위치 변수선언 
GLfloat mdx, mdy; //마우스놓은위치 변수선언
GLfloat tx, ty;
GLfloat tx_old = 0.0, ty_old = 0.0; //누적 Translation
GLfloat scale = 1.0, angle = 0.0, theta = 0.0;
GLfloat centerx, centery, centerz;
GLfloat bx_up = -1.0, by_up = -1.0, bx_down = 1.0, by_down = 1.0; //Bounding box of vertices
GLfloat fov, slope, dx, dy;
GLboolean isPerspective = false;
GLboolean transformCoordinateFrame = false;
GLfloat t[] = { 0.0, 0.0, 0.0 };

GLfloat WindowSizeWidth = 500.0;
GLfloat WindowSizeHeight = 500.0;

GLfloat axisRotateX = 0.0, axisRotateY = 0.0, axisRotateZ = 0.0; // Added for axis rotation
GLfloat axisX = 1.0, axisY = 1.0, axisZ = 1.0;

GLfloat M[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

void AddInput(int button, int state, int x, int y) {
    mx = (x - 250.) / 250.;
    my = (250. - y) / 250.;

    if (AddInputMode == 1 && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

        //Bounding box (y)
        if (my < by_down)
            by_down = my;
        if (my > by_up)
            by_up = my;

        //Bounding box (x)
        if (mx < bx_down)
            bx_down = mx;
        if (mx > bx_up)
            bx_up = mx;

        Vertex[NumVertex][0] = mx;
        Vertex[NumVertex][1] = my;
        Vertex[NumVertex][2] = 0.0;
        VertexBack[NumVertex][0] = mx; // Set the back face vertices with the same x and y
        VertexBack[NumVertex][1] = my;
        NumVertex++;
    }
    else if (AddInputMode == 2 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        glGetFloatv(GL_MODELVIEW_MATRIX, M);
        M[12] = 0.0;
        M[13] = 0.0;
        M[14] = 0.0;

        if (AddInputMode == 2)  //Accumulated translation with mouse relese in translation mode  
        {
            tx_old += tx;
            ty_old += ty;
        }
    }
    glutPostRedisplay();
}

void MouseDrag(int x, int y) {

    mdx = (x - 250.) / 250.;  //Mouse dragging position
    mdy = (250. - y) / 250.;

    tx = mdx - mx;  //Mouse dragging diatance
    ty = mdy - my;

    slope = (mdx - mx) / (mdy - my); //Slope of the normal line of mouse drag direction

    if (mdy < my) dx = sqrt(1.0 / (1. + slope * slope));  //Unit normal vector(x)
    else dx = -sqrt(1.0 / (1. + slope * slope));

    dy = -slope * dx;  //Unit normal vector(y)

    theta = atan(dy / dx) * 180. / 3.14;

    if (my == mdy) theta = 90.;
    if (mdy > my) theta += 180.;

    angle = sqrt((mdy - my) * (mdy - my) + (mdx - mx) * (mdx - mx)) * 180. / 3.14 * 4.;

    glMatrixMode(GL_MODELVIEW); //Set-up for the modeling & viewing matrix
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0, 1, 0);

    if (AddInputMode == 2) //Translation
    {
        theta = 0.0;
        angle = 0.0;
        scale = 1.0;
        glTranslatef(tx, ty, 0.0);
    }
    else if (AddInputMode == 3) //Rotation
    {
        scale = 1.0;

    }
    else if (AddInputMode == 4) //Rotation about Z
    {
        theta = 0.0;
        scale = 1.0;
    }
    else if (AddInputMode == 5) //Scaling
    {
        theta = 0.0;
        angle = 0.0;
        scale = abs(1.0 + ty / 2.0);
    }
    else if (AddInputMode == 6) { //Rotate Axis
        axisRotateX += (mdy - my) * 180. / 3.14;
        axisRotateY += (mdx - mx) * 180. / 3.14;
    }
    glTranslatef(tx_old, ty_old, 0.0);
    glTranslatef(centerx, centery, centerz);
    glRotatef(theta, 0., 0., 1.);
    if (AddInputMode == 3) glRotatef(angle, axisX, 0., 0.);
    if (AddInputMode == 4) glRotatef(-tx * 180. / 3.14, 0., 0., axisZ);
    if (AddInputMode == 6)
    {
        glRotatef(axisRotateX, 1.0, 0.0, 0.0);
        glRotatef(axisRotateY, 0.0, 1.0, 0.0);

        glPushMatrix();
        glLoadIdentity();
        glRotatef(axisRotateX, 1.0, 0.0, 0.0);
        glRotatef(axisRotateY, 0.0, 1.0, 0.0);
        GLfloat matrix[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
        axisX = matrix[0];
        axisY = matrix[4];
        axisZ = matrix[8];
        glPopMatrix();
    }
    glRotatef(-theta, 0., 0., 1.);
    glMultMatrixf(M);
    glScalef(scale, scale, scale);
    glTranslatef(-centerx, -centery, -centerz);

    //Redraw the scene in the display callback function
    glutPostRedisplay();
}

void Reshape(GLint width, GLint height)
{
    WindowSizeWidth = width;
    WindowSizeHeight = height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (isPerspective)
    {
        gluPerspective(230, 1.0, 0.1, 100.0); // FoV : yz 평면상의 각도. Aspect : 종횡비. Near, Far (-z)
        isPerspective = false;
    }
    else
    {
        glOrtho(-1.0, 1.0, -1.0, 1.0, -50.0, 50.0);
        isPerspective = true;
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawCoordinateFrame() {
    glPushMatrix();
    glLoadIdentity();
    glRotatef(theta, 0., 0., 1.);
    glRotatef(axisRotateX, 1.0, 0.0, 0.0);
    glRotatef(axisRotateY, 0.0, 1.0, 0.0);
    glRotatef(-theta, 0., 0., 1.);
    glLineWidth(2.0);

    // X-axis (Red)
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glEnd();

    // Y-axis (Green)
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glEnd();

    // Z-axis (Blue)
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glEnd();

    glLineWidth(1.0);
    glPopMatrix();
}


void drawCube() {
    
    int i;

    if (AddInputMode == 1) {

        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_POINTS);
        for (i = 0; i < NumVertex; i++) {
            glVertex3f(Vertex[i][0], Vertex[i][1], Vertex[i][2]);
        }
        glEnd();
    }

    else {

        // Back
        glColor3f(0.0, 0.0, 1.0);
        glBegin(GL_POLYGON);
        for (i = 0; i < NumVertex; i++) {
            glVertex3f(VertexBack[i][0], VertexBack[i][1], VertexBack[i][2]);
        }
        glEnd();

        // Line
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        for (i = 0; i < NumVertex; i++) {
            glVertex3f(Vertex[i][0], Vertex[i][1], Vertex[i][2]);
            glVertex3f(VertexBack[i][0], VertexBack[i][1], VertexBack[i][2]);
        }
        glEnd();

        // Front
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_POLYGON);
        for (i = 0; i < NumVertex; i++) {
            glVertex3f(Vertex[i][0], Vertex[i][1], Vertex[i][2]);
        }
        glEnd();

        // Point
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_POINTS);
        glVertex3f(centerx, centery, centerz);
        glEnd();

        drawCoordinateFrame();
    }


}



void Display() {
    int i;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawCube();
    
    glFlush();

}


//-----------------------------------------------------------------------------
// initiate graphics for the application
//-----------------------------------------------------------------------------
void InitGraphics(void)
{
    //Initialize graphics primitives
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);
    glShadeModel(GL_SMOOTH);
    glPointSize(5.0);

    //Set-up view volume for the application
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (!isPerspective) {
        glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 10.0);
    }
    else {
        fov = atan(0.5) * 180. / 3.14 * 2.0;   //viewer(0, 0, 5) -> (1, 1, 0):(-1, -1, 0)
        gluPerspective(fov, 1.0, 0.1, 10.0);
    }

    //Set-up the original view vector
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, M);
    gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0, 1, 0);
}



void MyMenu(int value)
{
    int i;

    switch (value) {
    case 1:
        AddInputMode = 0;
        centerx = 0.0;
        centery = 0.0;
        centerz = 0.0;
        for (i = 0; i < NumVertex; i++) {
            centerx += Vertex[i][0];
            centery += Vertex[i][1];
            VertexBack[i][2] = bx_up - bx_down;
            centerz = (bx_up - bx_down) / 2;
        }
        centerx /= NumVertex;
        centery /= NumVertex;
        break;
    case 2:
        AddInputMode = 2;
        break;
    case 3:
        AddInputMode = 3;
        break;
    case 4:
        AddInputMode = 4;
        break;
    case 5:
        AddInputMode = 5;
        break;
    case 6:
        AddInputMode = 6;
        break;
    case 7:
        NumVertex = 0;
        for (int i = 0; i < 50; i++)
        {
            Vertex[i][0] = 0.0;
            Vertex[i][1] = 0.0;
            Vertex[i][2] = 0.0;
            VertexBack[i][0] = 0.0;
            VertexBack[i][1] = 0.0;
            VertexBack[i][2] = 0.0;
        }

        mx = 0.0, my = 0.0;
        mdx = 0.0, mdy = 0.0;
        t[0] = 0.0, t[1] = 0.0, t[2] = 0.0;
        scale = 1.0;
        tx_old = 0.0, ty_old = 0.0;
        bx_up = -1.0; by_up = -1.0; bx_down = 1.0; by_down = 1.0;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, -50.0, 50.0);
        isPerspective = true;
        glMatrixMode(GL_MODELVIEW);

        AddInputMode = 1;
        glLoadIdentity();
        break;
    case 8:
        AddInputMode = 8;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluLookAt(0.0, 0.0, 0.2, 0.0, 0.0, -100.0, 0.0, 1.0, 0.0);
        glLoadIdentity();

        if (isPerspective)
        {
            gluPerspective(120, 1.0, 0.1, 10.0); // FoV : yz 평면상의 각도. Aspect : 종횡비. Near, Far (-z)
            cout << "Perspective" << endl;
            isPerspective = false;
        }
        else
        {
            glOrtho(-1.0, 1.0, -1.0, 1.0, -50.0, 50.0);
            cout << "Ortho" << endl;
            isPerspective = true;
        }
        glMatrixMode(GL_MODELVIEW);

        break;
    case 9:
        exit(0);
    case 10:
        
        glTranslatef(-tx_old, -ty_old, 0.0);
        glTranslatef(centerx, centery, centerz);
        glRotatef(-t[2], 0.0, 0.0, 1.0);
        glRotatef(-t[1], 0.0, 1.0, 0.0);
        glRotatef(-t[0], 1.0, 0.0, 0.0);
        glScalef(1.0 / scale, 1.0 / scale, 1.0 / scale);
        glTranslatef(-centerx, -centery, -centerz);

        mx = 0.0, my = 0.0;
        mdx = 0.0, mdy = 0.0;
        t[0] = 0.0, t[1] = 0.0, t[2] = 0.0;
        scale = 1.0;
        tx_old = 0.0, ty_old = 0.0;
        bx_up = -1.0; by_up = -1.0; bx_down = 1.0; by_down = 1.0;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, -0.1, 10.0);
        isPerspective = true;
        glMatrixMode(GL_MODELVIEW);
        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, -50.0, 50.0);
        isPerspective = true;
        glMatrixMode(GL_MODELVIEW);

        AddInputMode = 2;
        glLoadIdentity();
        InitGraphics();
    case 11:
        AddInputMode = 6; // 공간 회전 모드
        transformCoordinateFrame = !transformCoordinateFrame;
        break;

    }
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Assignment2");

    //Initialize OpenGL graphics state
    InitGraphics();

    glutCreateMenu(MyMenu);
    glutAddMenuEntry("Polygon", 1);
    glutAddMenuEntry("Translate", 2);
    glutAddMenuEntry("Rotate XY", 3);
    glutAddMenuEntry("Rotate Z", 4);
    glutAddMenuEntry("Scale", 5);
    glutAddMenuEntry("Clear", 7);
    glutAddMenuEntry("Perspective/Ortho", 8);
    glutAddMenuEntry("Exit", 9);
    glutAddMenuEntry("Reset", 10);
    glutAddMenuEntry("coordinate transformation", 11);

    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutDisplayFunc(Display);
    glutMouseFunc(AddInput);
    glutMotionFunc(MouseDrag);
    glutReshapeFunc(Reshape);

    glutMainLoop();
    return 0;
}