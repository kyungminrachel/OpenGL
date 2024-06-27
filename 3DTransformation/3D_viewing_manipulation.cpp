//-----------------------------------------------------------------------------
//           Name: 3D_viewing_manipulation.cpp
//         Author: Young Ho Chai
//  Last Modified: 05/16/16
//    Description: This sample demonstrates how adjustments to OpenGL's 
//                 viewing position effect the view.
//
//   Control: Left Mouse Click & Drag Up     - Viewer moves upward
//            Left Mouse Click & Drag Down   - Viewer moves downward
//            Left Mouse Click & Drag Left   - Viewer moves leftward
//            Left Mouse Click & Drag Right  - Viewer moves rightward
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <iostream>
#include <gl/freeglut.h>

GLfloat Vertex[20][3], VertexB[20][3]; //Vertex array for front & back faces
GLint NumVertex = 0;                   //Number of vertices
GLint AddInputMode = 1;                //1:Input, 2:Translation, 3:Orthographic/Perspective View
GLboolean ProjectionMode = GL_TRUE;    //TRUE:Orthographic, FALSE:Perspective
GLfloat mx, my;                        //Mouse picking position 
GLfloat mdx = 0.0, mdy = 0.0;          //Mouse dragging position
GLfloat tx, ty;                        //Amount of translation
GLfloat tx_old = 0.0, ty_old = 0.0;    //Accumulated translation
GLfloat centerx, centery, centerz;     //Center of the polygon
GLfloat bx_up = -1.0, by_up = -1.0, bx_lo = 1.0, by_lo = 1.0; //Bounding box of vertices
GLfloat fov, slope, dx, dy;

GLfloat scale = 1.0;
GLfloat theta = 0.0, angle = 0.0;
GLfloat M[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

//-----------------------------------------------------------------------------
// mouse click callback function
//-----------------------------------------------------------------------------
void AddInput(int button, int state, int x, int y){
	
	mx = (x - 250.) / 250.; //Mouse click position
	my = (250. - y) / 250.;

	if (AddInputMode == 1 && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (my < by_lo) by_lo = my;  //Bounding box calculation(y)
		if (my > by_up) by_up = my;

		if (mx < bx_lo) bx_lo = mx;  //Bounding box calculation(x)
		if (mx > bx_up) bx_up = mx;

		Vertex[NumVertex][0] = mx;   //Vertices for frontal face
		Vertex[NumVertex][1] = my;
		Vertex[NumVertex][2] = 0.0;
		VertexB[NumVertex][0] = mx;  //Vertices for back face
		VertexB[NumVertex][1] = my;

		NumVertex++;
	} 
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
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

	//Redraw the scene in the display callback function
	glutPostRedisplay();
}

//-----------------------------------------------------------------------------
// mouse drag callback function
//-----------------------------------------------------------------------------
void MouseDrag(int x, int y){

	mdx = (x - 250.) / 250.;  //Mouse dragging position
	mdy = (250. - y) / 250.;

	tx = mdx - mx;  //Mouse dragging diatance
	ty = mdy - my;
	
	slope = (mdx - mx) / (mdy - my); //Slope of the normal line of mouse drag direction

	if (mdy < my) dx = sqrt(1.0 / (1. + slope*slope));  //Unit normal vector(x)
	else dx = -sqrt(1.0 / (1. + slope*slope));

	dy = -slope*dx;  //Unit normal vector(y)

	theta = atan(dy / dx)*180. / 3.14;

	if (my == mdy) theta = 90.;
	if (mdy > my) theta += 180.;

	angle = sqrt((mdy - my)*(mdy - my) + (mdx - mx)*(mdx - mx))*180. / 3.14*4.;

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
		scale = abs(1.0 + ty/2.0);
	}

	glTranslatef(tx_old, ty_old, 0.0);
	glTranslatef(centerx, centery, centerz);
	glRotatef(theta, 0., 0., 1.);
	if (AddInputMode == 3) glRotatef(angle, 1., 0., 0.);
	if (AddInputMode == 4) glRotatef(-tx*180./3.14, 0., 0., 1.);
	glRotatef(-theta, 0., 0., 1.);
	glMultMatrixf(M);
	glScalef(scale, scale, scale);
	glTranslatef(-centerx, -centery, -centerz);

	//Redraw the scene in the display callback function
	glutPostRedisplay();
}

//-----------------------------------------------------------------------------
// display callback function
//-----------------------------------------------------------------------------
void Display()
{
	int i;

	//Clear the color & depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (AddInputMode == 1){ //Vertices only
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_POINTS);
		for (i = 0; i < NumVertex; i++){
			glVertex3f(Vertex[i][0], Vertex[i][1], Vertex[i][2]);
		}
		glEnd();
    }
	else{
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_POLYGON); //Back face
		for (i = 0; i < NumVertex; i++){
			glVertex3f(VertexB[i][0], VertexB[i][1], VertexB[i][2]);
		}
		glEnd();

		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES); //Lines vetween faces
		for (i = 0; i < NumVertex; i++){
			glVertex3f(Vertex[i][0], Vertex[i][1], Vertex[i][2]);
			glVertex3f(VertexB[i][0], VertexB[i][1], VertexB[i][2]);
		}
		glEnd();

		glPushMatrix(); //Case not considering the current ModelView matrix
			glLoadIdentity();
			glLineWidth(5.0);
			glColor3f(1.0, 0.0, 0.);
			glBegin(GL_LINES);  //Line for the mouse dragging
			glVertex3f(mx, my, 0.0);
			glVertex3f(mdx, mdy, 0.);
			glEnd();
			glTranslatef(mx, my, 0.);
			glColor3f(0.0, 1.0, 0.0);
			glBegin(GL_LINES); //Line perpendicular to the mouse dragging direction
			glVertex3f(dx, dy, 0.);
			glVertex3f(-dx, -dy, 0.);
			glEnd();
			glLineWidth(1.0);
		glPopMatrix();  //Return back to the current Modelview matrix

		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_POLYGON); //Front face
		for (i = 0; i < NumVertex; i++){
			glVertex3f(Vertex[i][0], Vertex[i][1], Vertex[i][2]);
		}
		glEnd();

		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_POINTS); //Center of the front face
			glVertex3f(centerx, centery, centerz);
		glEnd();
	}
	
	//Force execution of GL commands in finite time
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
	if (ProjectionMode){
		glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 10.0);
	}
	else{
		fov = atan(0.5)*180. / 3.14*2.0;   //viewer(0, 0, 5) -> (1, 1, 0):(-1, -1, 0)
		gluPerspective(fov, 1.0, 0.1, 10.0);
	}

	//Set-up the original view vector
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, M);
	gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0, 1, 0);
}

//-----------------------------------------------------------------------------
// menu function of a windows application
//-----------------------------------------------------------------------------
void MyMenu(int value)
{
	int i;

	switch (value){
	    case 1: //Calculate center of vertices
			AddInputMode = 0;
			centerx = 0.;
			centery = 0.;
			centerz = -(abs(bx_up - bx_lo) + abs(by_up - by_lo)) / 4.0;  //Half of the BB
			for (i = 0; i < NumVertex; i++){
				centerx += Vertex[i][0];
				centery += Vertex[i][1];
				VertexB[i][2] = centerz*2.;
			}
			centerx /= NumVertex;
			centery /= NumVertex;
			break;
		case 2: //Translation of the polygon
			AddInputMode = 2;
			break;
		case 3: //Rotation of the polygon
			AddInputMode = 3;
			break;
		case 4: //Rotation about the Z axis
			AddInputMode = 4;
			break;
		case 5: //Scaling of the polygon
			AddInputMode = 5;
			break;
		case 6: //Toggle the projection mode
			AddInputMode = 6;
			ProjectionMode = !ProjectionMode;
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			if (ProjectionMode){
				glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 10.0);
			}
			else{
				fov = atan(0.5)*180. / 3.14*2.0;   //viewer(0, 0, 5) -> (1, 1, 0):(-1, -1, 0)
				gluPerspective(fov, 1.0, 0.1, 10.0);
			}
			break;
		case 7: //Reset the view
			AddInputMode = 0;
			angle = 0;
			theta = 0;
			tx_old = 0.0;
			ty_old = 0.0;
			InitGraphics();
			break;
		case 8: //Clear the vertices & reset the view
			AddInputMode = 1;
			NumVertex = 0;
			ProjectionMode = GL_TRUE;
			tx_old = 0.0;
			ty_old = 0.0;
			bx_up = -1.0; by_up = -1.0; bx_lo = 1.0; by_lo = 1.0;
			InitGraphics();
			break;
		case 9:
			exit(0);
	}
	
	//Redraw the scene in the display callback function
	glutPostRedisplay();
}

//-----------------------------------------------------------------------------
// main function of a windows application
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	//Initialize FreeGLUT
	glutInit(&argc, argv);

	//Create Single Buffered Window
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
    glutCreateWindow("Assignment3");

	//Initialize OpenGL graphics state
	InitGraphics();

	//Register Callbacks:
	glutDisplayFunc(Display);
	glutMouseFunc(AddInput);
	glutMotionFunc(MouseDrag);
	
	//Create Pull-down Menu:
	glutCreateMenu(MyMenu);
	glutAddMenuEntry("Polygon", 1);
	glutAddMenuEntry("Translation", 2);
	glutAddMenuEntry("Rotate", 3);
	glutAddMenuEntry("Rotate about Z", 4);
	glutAddMenuEntry("Scale", 5);
	glutAddMenuEntry("Orthographic/Perspective", 6);
	glutAddMenuEntry("Reset", 7);
	glutAddMenuEntry("Clear", 8);
	glutAddMenuEntry("Exit", 9);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//Start GLUT main loop
	glutMainLoop();

	return 0;
}