#if defined(__APPLE__)
	#include <OpenGL/OpenGL.h>
	#include <GLUT/glut.h>
#else
	#include <GL/gl.h>
	#include <GL/freeglut.h>
	#include <GL/glu.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "model.h"
using namespace std;


#define INC 0.01
#define ANGLE_INC 0.25
#define PI 3.14159265358979

void initGL();
void refresh(void);
void resize(int w, int h);
void move(int x, int y);
void click(int a, int b, int x, int y);
void key(unsigned char a, int x, int y);
void pinta_model();
void pinta_terra();
void pinta_ninot();
void calcCapsa();
void tipus_cam();
void pos_cam();

vector<double> trans(3, 0.0);
vector<double> colours(4, 0.0);
vector<double> sca(3, 1.0);
vector<double> vrp(3, 0.0);
vector<double> v_capsa(3, 0.0);

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

//  aux_x and aux_y help control to check if a mouse action is a click or a movement
int aux_x = 0;
int aux_y = 0;
// alpha, beta control the angle sto rotate the image
double alpha = 0;
double beta = 0;
// win_x, win_y control the size of the window in pixels
int win_x = 600;
int win_y = 600;

// model
string filename = "legoman.obj";
Model m;
// capsa contenidora del model
double xmin, ymin, zmin;
double xmax, ymax, zmax;
// moviment del model en el pla xz
double modelx = -0.75;
double modelz = 0.75;

// capsa contenidora de l'escena
double escena_xmin, escena_ymin, escena_zmin;
double escena_xmax, escena_ymax, escena_zmax;
// esfera contenidora
double radi;
// valors de camera axonometrica
double xleft, xright, ybottom, ytop, zNear, zFar;
// valors de la camera perspectiva
double fovy, aspect, dist, alpha_v;
// valors de la posició de la camera
double anglex, angley, anglez;


void esfera_contenidora()
{
	// escena amb el terra, el ninot i el legoman a escala
	// centre de l'escena
	vrp[0] = vrp[1] = vrp[2] = 0;
	// vrp[1] = 1.2 / 2;
	// vertex de la capsa contenidora de tota l'escena mes allunyat de vrp
	if (vrp[0] - escena_xmin > vrp[0] - escena_xmax) v_capsa[0] = escena_xmin;
	else v_capsa[0] = escena_xmax;
	if (vrp[1] - escena_ymin > vrp[1] - escena_ymax) v_capsa[1] = escena_ymin;
	else v_capsa[1] = escena_ymax;
	if (vrp[2] - escena_zmin > vrp[2] - escena_zmax) v_capsa[2] = escena_zmin;
	else v_capsa[2] = escena_zmax;

	radi = sqrt(pow(v_capsa[0]-vrp[0], 2) + pow(v_capsa[1]-vrp[1], 2) + pow(v_capsa[2]-vrp[2], 2));
}

void tipus_cam()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
}

void pos_cam()
{
	glLoadIdentity();
	glTranslated(0.0, 0.0, -dist);
	glRotated(-anglez, 0, 0, 1);
	glRotated(anglex, 1, 0, 0);
	glRotated(-angley, 0, 1, 0);
	glTranslated(-vrp[0], -vrp[1], -vrp[2]);
}

void parametres_camera()
{
	dist = 2 * radi;
	alpha_v = asin(radi / dist);
	fovy = 2 * (alpha_v * 180 / PI);
	aspect = 1;
	zNear = radi;
	zFar = 3 * radi;
}

void initGL()
{
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	colours[1] = 0.50;
	glClearColor(colours[0], colours[1], colours[2], colours[3]);
	// inicialitzacions de camera
	angley = anglex = 45.0;
	escena_xmin = escena_zmin = -0.75;
	escena_xmax = escena_zmax = 0.75;
	escena_ymin = -0.4;
	escena_ymax = 0.8;
	esfera_contenidora();
	parametres_camera();
	tipus_cam();
	pos_cam();
}

void pinta_ninot()
{
	glPushMatrix();
		glPushMatrix();
			glColor3d(1, 1, 1);
			glutSolidSphere(0.4, 20, 20);

			glPushMatrix();
			glTranslated(0.0, 0.6, 0.0);
			glColor3d(1, 1, 1);
			glutSolidSphere(0.2, 20, 20);
			glPopMatrix();

			glPushMatrix();
			glTranslated(0.1, 0.6, 0);
			glRotated(90, 0, 1, 0);
			glColor3d(1, 0.698, 0.2353);
			glutSolidCone(0.1, 0.2, 20, 20);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

void pinta_terra()
{
	glPushMatrix();
	glBegin(GL_QUADS);
		glColor3d(0.6470, 0.2980, 0);
		glVertex3d(-0.75, -0.4, -0.75);
		glVertex3d(-0.75, -0.4, 0.75);
		glVertex3d(0.75, -0.4, 0.75);
		glVertex3d(0.75, -0.4, -0.75);
	glEnd();
	glPopMatrix();
}

void pinta_model()
{
	glPushMatrix();
		double altura = ymax - ymin;
		double escala = 0.5 / altura;
		if (modelx > 0.75) modelx = 0.75;
		if (modelx < -0.75+((xmax-xmin)*escala)) modelx = -0.75+((xmax-xmin)*escala);
		if (modelz > 0.75) modelz = 0.75;
		if (modelz < -0.75+(zmax-zmin)*escala) modelz = -0.75-(zmax-zmin)*escala;
		// escalar, desplasar
		glTranslated(modelx, -0.4, modelz);
		glScaled(escala, escala, escala);
		glTranslated(-xmax, -ymin, -zmax);
		for (int i = 0; i < m.faces().size(); i++) {
			const Face f = m.faces()[i];
				glColor4fv(Materials[f.mat].diffuse);
			glBegin(GL_POLYGON);
				for (int j = 0; j < f.v.size(); j++) {
					glVertex3dv(&m.vertices()[f.v[j]]);
				}
			glEnd();
		}
	glPopMatrix();
}

void refresh()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslated(trans[0], trans[1], trans[2]);
	glRotated(alpha, 0, 1, 0);
	glRotated(beta, 1, 0, 0);
	glScaled(sca[0], sca[1], sca[2]);
	pinta_terra();
	pinta_ninot();
	pinta_model();
	glPopMatrix();
	glutSwapBuffers();
}

void resize(int w, int h)
{
	win_x = w;
	win_y = h;
	if (w != h) {
		double ra_v = (double)w / (double)h;
		if (ra_v < 1) {
			alpha_v = atan(tan(alpha_v) / ra_v);
			fovy = 2 * alpha_v;
		}
		aspect = ra_v;
		tipus_cam();
	}
	glViewport(0, 0, w, h);
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
	} else if (state == 4) {
		modelx += 2 * (x - aux_x) / (double)win_x;
		modelz -= 2 * (aux_y - y) / (double)win_y;
		aux_x = x;
		aux_y = y;
	} else if (state == 5) {
		trans[0] += 2 * (x - aux_x) / (double)win_x;
		trans[1] += 2 * (aux_y - y) / (double)win_y;
		aux_x = x;
		aux_y = y;
	} else if (state == 6) {
		// fa el mateix que la rotació pero rotant la camera en comptes de l'escena
		angley += 360 * (2 * (x - aux_x) / (double)win_x);
		anglex += 360 * (2 * (aux_y - y) / (double)win_y);
		aux_x = x;
		aux_y = y;
		pos_cam();
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
		cout << "Pitjeu 'f' per per canviar el color\n";
		cout << "Pitjeu 'r' per modificar els angles girats de l'escena\n";
		cout << "Pitjeu 'e' per canviar els angles d'Euler\n";
		cout << "Pitjeu 't' per moure l'escena (si es mou del centre l'aplicacio pot no funcionar correctament)\n";
		cout << "Pitjeu 's' per escalar el model\n";
		cout << "Pitjeu 'm' per canviar el model" << endl;
		cout << "Pitjeu 'c' per moure el model, que es moura en la direccio del ratoli per el terra\n";
		cout << "Pitjeu ESC per sortir de l'aplicacio.\n";
	}
	else if (a == 'e') state = 6;
	else if (a == 't') state = 5;
	else if (a == 'c') state = 4;
	else if (a == 27) exit(0);
	else if (a == 'm') {
		cin >> filename;
		calcCapsa();
	}
	else if (a == 's') state = 3;
	else if (a == 'r') state = 2;
	else if (a == 'f') state = 1;
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
