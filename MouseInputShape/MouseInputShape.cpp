#include <GL/glut.h>
#include <vector>
#include <iostream>

using namespace std;

//좌표 저장 백터 
vector<float> vertices;



void drawPoints() {
    
    glPointSize(5.0f); 
    glBegin(GL_POINTS);
    for (int i = 0; i < vertices.size(); i += 2) {
        glVertex2f(vertices[i], vertices[i + 1]);
    }
    glColor3f(0.0, 1.0, 0.0);
    glEnd();
}

void display() {
    
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON); 

    for (int i = 0; i < vertices.size(); i += 2) {
        glVertex2f(vertices[i], vertices[i + 1]);

    }
    
    glEnd();
    drawPoints();
    glFlush();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        //정규화 
        //float normX = static_cast<float>(x) / static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) * 2.0f - 1.0f;
        //float normX = 2 ( static_cast<float>(x)/ glutGet(GLUT_WINDOW_WIDTH) * 2.0f - 1.0f);

        float normX = static_cast<float>(x)/ static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) * 4.0f - 2.0f;
        float normY = 1.0f - static_cast<float>(y) / static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) * 2.0f;
        



        cout << normX << " " << normY << endl;

        vertices.push_back(normX);
        vertices.push_back(normY);

        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  //순서가 통상적인 코드와 반대다
    glutInitWindowSize(1000, 500);
    glutCreateWindow("Mouse Shape");
    gluOrtho2D(-2.0, 2.0, -1.0, 1.0);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);

    glutMainLoop();

    return 0;
}