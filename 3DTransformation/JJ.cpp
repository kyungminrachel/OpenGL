#include <stdio.h>
#include <iostream>
#include <GL/freeglut.h>
using namespace std;

GLfloat Vertex[50][3];
GLfloat VertexZ[50][3];
GLint NumVertex = 0;
GLint AddInputMode = 1; // 1: Input, 2: Translation, 3: Rotation, 4: Scaling
GLint ViewInputMode = 0; // 0: ortho, 1: Perspective
GLfloat RoAxisX, RoAxisY = 0., RoAxisZ = 1.;
GLfloat mx, my;
GLfloat mdx, mdy;
GLfloat WindowSizeWidth = 500.0;
GLfloat WindowSizeHeight = 500.0;
GLfloat tx, ty, tz;
GLfloat tx_old = 0.0, ty_old = 0.0, tz_old = 0.0;

GLfloat scale = 1.0, angle = 0.0;
GLfloat centerx, centery, centerz, centerx_auto, centery_auto, centerz_auto;
GLfloat AutoZpoinT = 1.;
GLfloat bx_up = -1.0, by_up = -1.0, bx_down = 1.0, by_down = 1.0; //Bounding box of vertices
GLboolean isPerspective = false;

GLfloat t[] = { 0.0, 0.0, 0.0 };

void AddInput(int button, int state, int x, int y) {

	mx = (x * 2.0 / WindowSizeWidth - 1.0);
	my = -((y * 2.0 / WindowSizeHeight) - 1.0);

	if (AddInputMode == 1 && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (my < by_down)
			by_down = my;
		if (my > by_up)
			by_up = my;

		if (mx < bx_down)
			bx_down = mx;
		if (mx > bx_up)
			bx_up = mx;

		Vertex[NumVertex][0] = mx;
		Vertex[NumVertex][1] = my;
		Vertex[NumVertex][2] = 0.0;

		VertexZ[NumVertex][0] = mx;
		VertexZ[NumVertex][1] = my;

		NumVertex++;
	}
	else if (AddInputMode == 2 && button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		tx_old += tx;
		ty_old += ty;
	}
	glutPostRedisplay();
}

void MouseDrag(int x, int y) {
	GLfloat centertx, centerty;

	mdx = (x * 2.0 / WindowSizeWidth - 1.0);
	mdy = -((y * 2.0 / WindowSizeHeight) - 1.0);

	glLoadIdentity();
	gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, -100.0, 0.0, 1.0, 0.0);
	if (AddInputMode == 2)
	{
		tx = mdx - mx;
		ty = mdy - my;

		glTranslatef(tx, ty, 0.0);
	}
	else if (AddInputMode == 3)
	{
		t[0] += -(mdx - mx) * 10.0 / 3.14;
	}
	else if (AddInputMode == 4)
	{
		t[1] += -(mdx - mx) * 10.0 / 3.14;
	}
	else if (AddInputMode == 5)
	{
		t[2] += -(mdx - mx) * 10.0 / 3.14;
	}
	else if (AddInputMode == 6)
	{
		scale *= (1.0 + (mdy - my) / 100.0);
	}

	glTranslatef(tx_old, ty_old, 0.0);
	glTranslatef(centerx, centery, centerz);

	glRotatef(t[0], 1.0, 0.0, 0.0);
	glRotatef(t[1], 0.0, 1.0, 0.0);
	glRotatef(t[2], 0.0, 0.0, 1.0);

	glScalef(scale, scale, scale);
	glTranslatef(-centerx, -centery, -centerz);
	glutPostRedisplay();

}
void Reshape(GLint width, GLint height)

{
	WindowSizeWidth = width;
	WindowSizeHeight = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (ViewInputMode == 1)
	{
		gluPerspective(60, 1.0, 0.01, 5.0); // FoV : yz 평면상의 각도. Aspect : 종횡비. Near, Far (-z)
	}
	else if (ViewInputMode == 0)

	{
		glOrtho(-1.0, 1.0, -1.0, 1.0, -2.0, 2.0);
	}
	glMatrixMode(GL_MODELVIEW);
}

void Display()
{
	int i;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_FLAT);
	glPointSize(5.0);

	if (AddInputMode == 1)
	{
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_POINTS);
		for (i = 0; i < NumVertex; i++) {
			glVertex3f(Vertex[i][0], Vertex[i][1], Vertex[i][2]);
		}
		glEnd();
	}

	else
	{
		// Line
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
		for (i = 0; i < NumVertex; i++)
		{
			glVertex3f(Vertex[i][0], Vertex[i][1], Vertex[i][2]);
			glVertex3f(VertexZ[i][0], VertexZ[i][1], VertexZ[i][2]);
		}
		glEnd();

		// Point
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_POINTS);
		glVertex3f(centerx, centery, centerz);
		glEnd();

		// Back
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_POLYGON);
		for (i = 0; i < NumVertex; i++)
		{
			glVertex3f(VertexZ[i][0], VertexZ[i][1], VertexZ[i][2]);
		}
		glEnd();

		// Front
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_POLYGON);
		for (i = 0; i < NumVertex; i++)
		{
			glVertex3f(Vertex[i][0], Vertex[i][1], Vertex[i][2]);
		}
		glEnd();
	}
	glFlush();
}

void MyMenu(int value)
{
	int i;

	switch (value) {
	case 1:
		AddInputMode = 0;
		centerx = 0.0;
		centery = 0.0;
		centerz = -(abs(bx_up - bx_down) + abs(by_up - by_down)) / 4.0;
		for (i = 0; i < NumVertex; i++) {
			centerx += Vertex[i][0];
			centery += Vertex[i][1];
			VertexZ[i][2] = centerz * 2.;
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
		AddInputMode = 1;
		angle = 0.0;
		scale = 1.0;
		tx_old = 0.0;
		ty_old = 0.0;
		bx_up = -1.0; by_up = -1.0; bx_down = 1.0; by_down = 1.0;
		glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 10.0);
		isPerspective = true;
		glLoadIdentity();
		glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 10.0);

		break;
	case 9:
		ViewInputMode = 0; // Orthographic 투영 모드
		Reshape(WindowSizeWidth, WindowSizeHeight); // Reshape 함수 호출
		break;
	case 10:
		ViewInputMode = 1; // Perspective 투영 모드
		Reshape(WindowSizeWidth, WindowSizeHeight); // Reshape 함수 호출
		break;
	case 11:
		exit(0);
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WindowSizeWidth, WindowSizeHeight);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("3D Transformation");
	glutCreateMenu(MyMenu);
	glutAddMenuEntry("Polygon", 1);
	glutAddMenuEntry("Translate", 2);
	glutAddMenuEntry("Rotate x", 3);
	glutAddMenuEntry("Rotate y", 4);
	glutAddMenuEntry("Rotate z", 5);
	glutAddMenuEntry("Scale", 6);
	glutAddMenuEntry("Clear", 7);
	glutAddMenuEntry("Reset", 8);
	glutAddMenuEntry("Ortho", 9);
	glutAddMenuEntry("Perspective", 10);
	glutAddMenuEntry("Exit", 11);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMotionFunc(MouseDrag);

	glutDisplayFunc(Display);

	glutMouseFunc(AddInput);

	glutReshapeFunc(Reshape);
	glutMainLoop();
	return 0;
}