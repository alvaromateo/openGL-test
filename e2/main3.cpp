#if defined(__APPLE__)
	#include <OpenGL/OpenGL.h>
	#include <GLUT/glut.h>
#else
	#include <GL/gl.h>
	#include <GL/freeglut.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include "model.h"
using namespace std;


#define INC 0.01
#define ANGLE_INC 0.25


void initGL();
void refresh(void);
void resize(int w, int h);
void move(int x, int y);
void click(int a, int b, int x, int y);
void key(unsigned char a, int x, int y);
void pinta_objectes();

vector<double> colours(4, 0.0);
vector<double> sca(3, 1.0);

/*  state = 0 --> canvia la traslacio
	state = 1 --> canvia els colors
	state = 2 --> canvia els angles de rotacio
*/
int state = 0;
// 	order controls the transformation to be done and the order
int order = 0;

// 	col controls the current colour to be modified
int col = 1;
// 	fade controls if the colour must get brighter or darker
int fade = 0;


//  aux_x and aux_y help control to check if a mouse action is
//  a click or a movement
int aux_x = 0;
int aux_y = 0;

// alpha, beta control the angle sto rotate the image
double alpha = 0;
double beta = 0;

// win_x, win_y control the size of the window in pixels
int win_x = 600;
int win_y = 600;

// model
string filename = "HomerProves.obj";
Model m;

// capsa contenidora del model
double xmin, ymin, zmin;
double xmax, ymax, zmax;

// controladors de la manera de pintar
bool enabled = false;
bool line = false;


void initGL()
{
	colours[1] = 0.50;
	glClearColor(colours[0], colours[1], colours[2], colours[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1,1,-1,1,-1,1);
	glMatrixMode(GL_MODELVIEW);
}

void pinta_objectes()
{
	for (int i = 0; i < m.faces().size(); i++) {
		const Face f = m.faces()[i];
		glBegin(GL_POLYGON);
			for (int j = 0; j < f.v.size(); j++) {
				glVertex3dv(&m.vertices()[f.v[j]]);
			}
		glEnd();
	}
}

void refresh()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (line) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (enabled) glDisable(GL_DEPTH_TEST);
	else glEnable(GL_DEPTH_TEST);

	glLoadIdentity();
	glRotated(alpha, 0, 1, 0);
	glRotated(beta, 1, 0, 0);
	glScaled(sca[0], sca[1], sca[2]);
	pinta_objectes();
	glutSwapBuffers();
}

void resize(int w, int h)
{
	win_x = w;
	win_y = h;
	int x, y, side;
	x = y = side = 0;
	if (w > h) {
		side = h;
		x = (w - side) / 2;
	} else {
		side = w;
		y = (h - side) / 2;
	}
	glViewport(x, y, side, side);
}

void click(int a, int b, int x, int y)
{
	if (b == GLUT_DOWN)
	{
		aux_x = x;
		aux_y = y;
	}
	if (b == GLUT_UP && aux_y == y && aux_x == x)
	{
		if (state == 1) {
			++col;
			col = col % 3;
			colours[0] = colours[1] = colours[2] = 0;
			if (col == 0) colours[0] = 1;
			if (col == 1) colours[1] = 1;
			if (col == 2) colours[2] = 1;

			glClearColor(colours[0], colours[1], colours[2], 0);
			glutPostRedisplay();
		}
		aux_x = aux_y = 0;
	}
}

void move(int x, int y)
{
	if (state == 1) {
		if (colours[col] <= 0.2) fade = 1;
		if (colours[col] >= 1.0) fade = 0;

		if (fade) colours[col] += INC;
		else colours[col] -= INC;
		glClearColor(colours[0], colours[1], colours[2], 0);
	} else if (state == 2) {
		alpha += 360 * (2 * (x - aux_x) / (double)win_x);
		beta += 360 * (2 * (aux_y - y) / (double)win_y);
		aux_x = x;
		aux_y = y;	
	} else if (state == 3) {
		sca[0] += 2 * (x - aux_x) / (double)win_x;
		sca[1] += 2 * (aux_y - y) / (double)win_y;
		aux_x = x;
		aux_y = y;
	}
	glutPostRedisplay();
}

void calcCapsa()
{
	m.load(filename);
	xmin = xmax = m.vertices()[0];
	ymin = ymax = m.vertices()[1];
	zmin = zmax = m.vertices()[2];

	for (int i = 0; i < m.vertices().size(); i += 3) {
		if (m.vertices()[i] < xmin) xmin = m.vertices()[i];
		if (m.vertices()[i] > xmax) xmax = m.vertices()[i];
		if (m.vertices()[i+1] < ymin) ymin = m.vertices()[i+1];
		if (m.vertices()[i+1] > ymax) ymax = m.vertices()[i+1];
		if (m.vertices()[i+2] < zmin) zmin = m.vertices()[i+2];
		if (m.vertices()[i+2] > zmax) zmax = m.vertices()[i+2];
	}
}

void key(unsigned char a, int x, int y)
{
	if (a == 'h') {
		cout << "Pitjeu 'n' per tornar al mode normal\n";
		cout << "Pitjeu 'h' per mostrar l'ajuda\n";
		cout << "Pitjeu 'c' per obrir el mode per canviar el color\n";
		cout << "Pitjeu 'r' per modificar els angles girats de l'escena\n";
		cout << "Pitjeu 's' per escalar el model\n";
		cout << "Pitjeu 'p' per canviar el mode de pintat\n";
		cout << "Pitjeu 'd' per activar/desactivar el mode z-buffer" << endl;
		cout << "Pitjeu 'm' per canviar el model" << endl;
		cout << "Pitjeu ESC per sortir de l'aplicacio.\n";
	}
	else if (a == 27) exit(0);
	else if (a == 'm') {
		cin >> filename;
		calcCapsa();
	}
	else if (a == 'd') {
		if (enabled) enabled = false;
		else enabled = true;
	}
	else if (a == 'p') {
		if (line) line = false;
		else line = true;
	}
	else if (a == 's') state = 3;
	else if (a == 'r') state = 2;
	else if (a == 'c') state = 1;
	else if (a == 'n') state = 0;
	glutPostRedisplay();
}

int main(int argc, const char *argv[])
{
	glutInit(&argc, (char **)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("IDI: Practiques OpenGL");
	initGL();
	calcCapsa();
	glutDisplayFunc(refresh);
	glutMouseFunc(click);
	glutMotionFunc(move);
	glutReshapeFunc(resize);
	glutKeyboardFunc(key);
	glutMainLoop();
	return 0;
}
