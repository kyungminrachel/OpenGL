#include <stdio.h>
#include <fstream>
#include <iostream>
#include <GL/freeglut.h>  
#include <windows.h>
#include <cmath>
#include <math.h>
using namespace std;

const int maxVertex = 100;

GLint windowWidth = 500, windowHeight = 500;							// Width and Height of window
GLfloat halfWindowWidth = static_cast<GLfloat>(windowWidth) / 2;
GLfloat halfWindowHeight = static_cast<GLfloat>(windowHeight) / 2;
GLfloat Vertex[maxVertex][2];											// Save vertex coordinates (x,y) in an array
GLfloat OriginalVertex[maxVertex][2];									// Save the original vertices
GLfloat Vertex3D[maxVertex][3];											// 3D point vertex (x,y,z)
GLint numVertex = 0;													// Number of points
GLfloat polygonCenterX, polygonCenterY, polygonCenterZ;
GLfloat maxValue;
GLint operation = 0;													// 1: Add by mouse and keyboard, 2: Translation, 3: Rotation, 4: Scale, 5: Ortho, 6: Perspective 10: Reset(Any value other than 1 can generate polygon and center point)
GLfloat mouseClickX, mouseClickY;										// Mouse Click Position
GLfloat mouseLastX, mouseLastY;											// Previous frame's mouse position
GLfloat mouseDragX, mouseDragY;											// Mouse Drag Position
GLfloat mouseDeltaX, mouseDeltaY;										// Delta translation of mouse position in window
GLfloat sumTranslationX = 0.0, sumTranslationY = 0.0;					// Sum Translation
GLfloat sumRotation = 0.0;
GLfloat sumScale = 1.0;
GLfloat keyboardDeltaX = 0.0, keyboardDeltaY = 0.0;						// Delta translation of temporary point input by keyboard
GLfloat scale = 1.0;
GLfloat angleXOY = 0.0, angleXOZ = 0.0, angleYOZ = 0.0;
GLfloat sumAngleXOY = 0.0, sumAngleXOZ = 0.0, sumAngleYOZ = 0.0;
GLfloat rotateSensativity = 10.0, scaleSensativity = 1.0;				// Mouse dragging sensativity
GLfloat rotateWheelSensativity = 1.0, scaleWheelSensativity = 10;		// Mouse wheel sensativity
GLfloat tempPointMovement = 0.1;										// Unit of translation of temporary point
GLfloat FOV = 90.0;
bool pointAddedByKeyboard = false;
const char* fileName = "MyCoordinates.txt";

GLfloat transform[4] = { sumTranslationX,sumTranslationY,sumRotation,sumScale };

// Reset some necessary parameters
void reset()
{
	for (int i = 0; i < numVertex; i++) {
		OriginalVertex[i][0] = 0.0;
		OriginalVertex[i][1] = 0.0;
	}
	numVertex = 0;
	operation = 1;
	angleXOY = 0.0;
	angleXOZ = 0.0;
	angleYOZ = 0.0;
	scale = 1.0;
	sumTranslationX = 0.0;
	sumTranslationY = 0.0;
	//sumRotation = 0.0;
	//sumAngleXOY = 0.0;
	//sumAngleXOZ = 0.0;
	//sumAngleYOZ = 0.0;
	//sumScale = 1.0;
}

// Read data from a txt file


// Mouse click and press space to add point, mouse click callback function
void mouseClick(int button, int state, int x, int y)
{
	// Coordinates when the left mouse button is pressed
	if (windowWidth <= windowHeight) {
		mouseClickX = (x - halfWindowWidth) / halfWindowWidth;
		mouseClickY = (halfWindowHeight - y) / halfWindowHeight * (halfWindowHeight / halfWindowWidth);
	}
	else {
		mouseClickX = (x - halfWindowWidth) / halfWindowWidth * (halfWindowWidth / halfWindowHeight);
		mouseClickY = (halfWindowHeight - y) / halfWindowHeight;
	}

	mouseLastX = mouseClickX;
	mouseLastY = mouseClickY;

	// Generate a point
	if (operation == 1 && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !pointAddedByKeyboard && numVertex < maxVertex)
	{
		//cout << mouseClickX << "," << mouseClickY << "\n";
		Vertex[numVertex][0] = mouseClickX;
		Vertex[numVertex][1] = mouseClickY;
		OriginalVertex[numVertex][0] = Vertex[numVertex][0];
		OriginalVertex[numVertex][1] = Vertex[numVertex][1];
		numVertex++;
	}
	// Translation
	else if (operation == 2 && button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		// Because mouse may not be pressed at the same position when it was released last time, so sum translation so far should be recorded.
		sumTranslationX += mouseDeltaX;
		sumTranslationY += mouseDeltaY;
	}
	// Rotation
	/*
	else if (operation == 3 && button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		cout << angleXOZ << "\n";
		cout << angleYOZ << "\n";
	}
	*/

	// Scale
	/*
	else if (operation == 4 && button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		cout << scale << "\n";
	}
	*/
	glutPostRedisplay();// Repaint window.

}

// Mouse drag callback function
void mouseDrag(int x, int y)
{
	// Coordinate after mouse is pressed and then moved
	if (windowWidth <= windowHeight) {
		mouseDragX = (x - halfWindowWidth) / halfWindowWidth;
		mouseDragY = (halfWindowHeight - y) / halfWindowHeight * (halfWindowHeight / halfWindowWidth);
	}
	else {
		mouseDragX = (x - halfWindowWidth) / halfWindowWidth * (halfWindowWidth / halfWindowHeight);
		mouseDragY = (halfWindowHeight - y) / halfWindowHeight;
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Translation
	if (operation == 2)
	{
		mouseDeltaX = mouseDragX - mouseClickX;
		mouseDeltaY = mouseDragY - mouseClickY;

		glTranslatef(mouseDeltaX, mouseDeltaY, 0.0);
	}

	// Rotation
	else if (operation == 3)
	{
		angleXOZ += (mouseLastX - mouseDragX) * rotateSensativity;// Rotation follows the right-hand rule, that is, counterclockwise rotation, so if you want a positive value to move clockwise, you need to invert it.
		angleYOZ += (mouseDragY - mouseLastY) * rotateSensativity;
		mouseLastX = mouseDragX;
		mouseLastY = mouseDragY;
		//sumRotation += angle;
		//sumAngleXOZ += angleXOZ;
		//sumAngleYOZ += angleYOZ;
	}

	// Scale
	else if (operation == 4)
	{
		scale *= (1.0 + (mouseDragY - mouseLastY) * scaleSensativity);
		mouseLastX = mouseDragX;
		mouseLastY = mouseDragY;
		//sumScale *= scale;
	}

	// Ortho/Perspective look at object
	else if (operation == 5)
	{
		gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}

	else if (operation == 11)
	{
		angleXOY += (mouseLastX - mouseDragX) * rotateSensativity;// Rotation follows the right-hand rule, that is, counterclockwise rotation, so if you want a positive value to move clockwise, you need to invert it.
		mouseLastX = mouseDragX;
	}

	glTranslatef(sumTranslationX, sumTranslationY, 0.0);
	glTranslatef(polygonCenterX, polygonCenterY, 0.0);
	glRotatef(angleXOY, 0.0, 0.0, 1.0); // Rotate about z
	glRotatef(angleXOZ, 0.0, 1.0, 0.0); // Rotate
	glRotatef(angleYOZ, 1.0, 0.0, 0.0); // Rotate
	glScalef(scale, scale, scale);
	glTranslatef(-polygonCenterX, -polygonCenterY, 0.0);
	glutPostRedisplay();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_FLAT);
	glPointSize(5.0);

	// Display point when point is added
	if (operation == 1)
	{
		// Keyboard space to display temporary point
		if (pointAddedByKeyboard)
		{
			glBegin(GL_POINTS);
			glColor3f(0.0, 0.0, 1.0);//RGB of temporary points (blue)
			glVertex2f(keyboardDeltaX, keyboardDeltaY);
			glEnd();
		}

		// Mouse click to display point
		glBegin(GL_POINTS);
		glColor3f(0.0, 1.0, 0.0);//RGB of points (green)
		for (int i = 0; i < numVertex; i++) {
			glVertex2f(Vertex[i][0], Vertex[i][1]);
		}
		glEnd();
	}

	// Display polygon and center point when point is no longer added
	else {
		// Side polygons
		/*
		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0, 1.0, 0.0);//RGB of lines (yellow)
		for (int i = 0; i < numVertex; i++) {
			glVertex3f(Vertex[i][0], Vertex[i][1], -maxValue);
			glVertex3f(Vertex[i][0], Vertex[i][1], maxValue);
		}
		glVertex3f(Vertex[0][0], Vertex[0][1], -maxValue);
		glVertex3f(Vertex[0][0], Vertex[0][1], maxValue);
		glEnd();
		*/
		// Back polygon
		glBegin(GL_POLYGON);
		glColor3f(0.0, 1.0, 0.0);//RGB of front polygon (green)
		// Connect points in order and form a polygon
		for (int i = 0; i < numVertex; i++) {
			glVertex3f(Vertex[i][0], Vertex[i][1], -maxValue);
		}
		glEnd();

		// Front polygon
		glBegin(GL_POLYGON);
		glColor3f(0.0, 0.0, 1.0);//RGB of back polygon (blue)
		for (int i = 0; i < numVertex; i++) {
			glVertex3f(Vertex[i][0], Vertex[i][1], maxValue);
		}
		glEnd();

		// Lines
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);//RGB of lines (blue)
		for (int i = 0; i < numVertex; i++) {
			glVertex3f(Vertex[i][0], Vertex[i][1], -maxValue);
			glVertex3f(Vertex[i][0], Vertex[i][1], maxValue);
		}
		glEnd();

		// Center point
		glBegin(GL_POINTS);
		glColor3f(1.0, 0.0, 0.0);//RGB of center point (red)
		glVertex3f(polygonCenterX, polygonCenterY, polygonCenterZ);
		glEnd();

	}

	glFlush();// Refresh the buffer and force the command to be sent to the driver for processing
}

// Calculate the center of polygon
void calculateCenter()
{
	polygonCenterX = 0.;
	polygonCenterY = 0.;
	for (int i = 0; i < numVertex; i++) {
		polygonCenterX += Vertex[i][0];
		polygonCenterY += Vertex[i][1];
	}
	polygonCenterX /= numVertex;
	polygonCenterY /= numVertex;
}

// Reset vertices
void resetVertices()
{
	/*
	for (int i = 0; i < numVertex; i++) {
		Vertex[i][0] = OriginalVertex[i][0];
		Vertex[i][1] = OriginalVertex[i][1];
	}
	*/
	glTranslatef(-sumTranslationX, -sumTranslationY, 0.0);
	glTranslatef(polygonCenterX, polygonCenterY, 0.0);
	glRotatef(-angleXOY, 0.0, 0.0, 1.0);
	glRotatef(-angleXOZ, 0.0, 1.0, 0.0);
	glRotatef(-angleYOZ, 1.0, 0.0, 0.0);
	glScalef(1.0 / scale, 1.0 / scale, 1.0 / scale);
	glTranslatef(-polygonCenterX, -polygonCenterY, 0.0);
}

// Right-click menu generation
void rightMenu(int option)
{
	// No operation will be done if the temporary point still exists.
	if (!pointAddedByKeyboard) {
		// The corresponding response event of each menu button
		switch (option) {
			// Generate polygon from txt file
		case 0:
			reset();
			glLoadIdentity();
			//generateFromFile(fileName);
			calculateCenter();
			operation = 0;
			break;

			// Generate polygon and no longer add points
		case 1:
			operation = 0;
			calculateCenter();
			break;
			// Translate
		case 2:
			operation = 2;
			calculateCenter();
			break;
			// Rotate
		case 3:
			operation = 3;
			calculateCenter();
			break;
			// Scale
		case 4:
			operation = 4;
			calculateCenter();
			break;

			// Clear and reset
		case 5:
			reset();
			glLoadIdentity();
			break;

		case 6:
			exit(0);

		case 7:
			operation = 5;
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-1.0, 1.0, -1.0, 1.0, -10.0, 10.0);
			break;
		case 8:
			operation = 5;
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(FOV, 1.0, 0.1, 10.0);
			break;
			// Reset View
		case 9:
			operation = 9;
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-1.0, 1.0, -1.0, 1.0, -10.0, 10.0);
			gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 4.0, 0.0, 1.0, 0.0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			break;
			// Reset Polygon
		case 10:
			operation = 10;
			resetVertices();
			calculateCenter();
			sumTranslationX = 0.0;
			sumTranslationY = 0.0;
			angleXOY = 0.0;
			angleXOZ = 0.0;
			angleYOZ = 0.0;
			scale = 1.0;
			glLoadIdentity();
			break;
		case 11:
			operation = 11;

		}
		glutPostRedisplay();// Repaint window
	}
}
/*
void reshape(int width, int height) {
	windowWidth = width;
	windowHeight = height;
	halfWindowWidth = static_cast<GLfloat>(width) / 2;
	halfWindowHeight = static_cast<GLfloat>(height) / 2;
	glViewport(0,0,(GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (width <= height)
	{
		glOrtho(-1.0, 1.0, -1.0 * (GLfloat)height / (GLfloat)width, 1.0 * (GLfloat)height / (GLfloat)width, -100.0, 100.0);
		glutPostRedisplay();
	}
	else
	{
		glOrtho(-1.0 * (GLfloat)width / (GLfloat)height, 1.0 * (GLfloat)width / (GLfloat)height, -1.0, 1.0, -100.0, 100.0);
		glutPostRedisplay();
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}
*/
int main(int argc, char** argv)
{
	// Read data from txt file at first and only be implemeted once in main function
	//generateFromFile(fileName);

	// Initiate window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Warm_up");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutCreateMenu(rightMenu);
	glutAddMenuEntry("File", 0);
	glutAddMenuEntry("Polygon", 1);
	glutAddMenuEntry("Translate", 2);
	glutAddMenuEntry("Rotate", 3);
	glutAddMenuEntry("Scale", 4);
	glutAddMenuEntry("Clear", 5);
	glutAddMenuEntry("Exit", 6);
	glutAddMenuEntry("Ortho", 7);
	glutAddMenuEntry("Perspective", 8);
	glutAddMenuEntry("Reset View", 9);
	glutAddMenuEntry("Reset Polygon", 10);
	glutAddMenuEntry("Rotate about z", 11);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Register callback functions
	glutDisplayFunc(display);
	//glutReshapeFunc(reshape);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseDrag);
	glEnable(GL_DEPTH_TEST);
	// Enter the main event processing loop of GLUT.
	glutMainLoop();
	return 0;
}