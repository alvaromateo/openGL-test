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
#define MOV 0.05
#define PI 3.14159265358979

void initGL();
void refresh(void);
void resize(int w, int h);
void move(int x, int y);
void click(int a, int b, int x, int y);
void key(unsigned char a, int x, int y);
void pinta_model(int num_model);
void pinta_terra();
void pinta_ninot();
void calcCapsa();
void tipus_cam();
void pos_cam();

vector<double> colours(4, 0.0);
vector<double> vrp(3, 0.0);
vector<double> v_capsa(3, 0.0);

vector<double> obs(3, 0.0);
vector<double> center(3, 0.0);

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
// win_x, win_y control the size of the window in pixels
int win_x = 600;
int win_y = 600;

// model
string filename = "Patricio.obj";
Model m;
// capsa contenidora del model
double xmin, ymin, zmin;
double xmax, ymax, zmax;
// moviment del model en el pla xz
double modelx = 0.0;
double modelz = 0.0;

// capsa contenidora de l'escena
double escena_xmin, escena_ymin, escena_zmin;
double escena_xmax, escena_ymax, escena_zmax;
// esfera contenidora
double radi;
// tipus de camera: 0 --> axonometrica, 1 --> perspectiva
// walk: 0 --> 3a persona, 1 --> 1a persona
int tipus, walk;
// valors de camera axonometrica
double xleft, xright, ybottom, ytop, zNear, zFar;
// valors de la camera perspectiva
double fovy, aspect, dist, alpha_v;
// valors de la posició de la camera
double anglex, angley, anglez;
double zoom;

// parets visibles
int visibilitat;
// velocitat del moviment
int vel;
/* direcció a la que apunta el patricio:
	0 --> -x
	1 --> +z
	2 --> +x
	3 --> -z
*/
int dir = 0;

// ILUMINACIO
int iluminacio = 1;


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

void tipus_cam(int tipus)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (tipus == 0) {
		// camera axonometrica
		glOrtho(xleft, xright, ybottom, ytop, zNear, zFar);
	} else if (tipus == 1) {
		// camera perspectiva
		gluPerspective(fovy, aspect, zNear, zFar);
	}
	glMatrixMode(GL_MODELVIEW);
}

void pos_cam(int tipus)
{
	double aux;
	if (tipus == 0) aux = -radi;
	else aux = -dist;
	aux += zoom;
	glLoadIdentity();
	if (walk) {
		if (dir == 0) {
			center[0] = modelx - 4.0;
			center[2] = modelz;
		} else if (dir == 1) {
			center[0] = modelx;
			center[2] = modelz + 4.0;
		} else if (dir == 2) {
			center[0] = modelx + 4.0;
			center[2] = modelz;
		} else {
			center[0] = modelx;
			center[2] = modelz - 4.0;
		}
		gluLookAt(modelx, 1.0, modelz, center[0], center[1], center[2], 0, 1, 0);
	} else {
		glTranslated(0.0, 0.0, aux);
		glRotated(-anglez, 0, 0, 1);
		glRotated(anglex, 1, 0, 0);
		glRotated(-angley, 0, 1, 0);
		glTranslated(-vrp[0], -vrp[1], -vrp[2]);
	}
}

void inicialitzacions()
{
	anglez = 0.0;
	angley = anglex = 45.0;
	escena_xmin = escena_zmin = -5.0;
	escena_xmax = escena_zmax = 5.0;
	escena_ymin = 0.0;
	escena_ymax = 1.5;
	esfera_contenidora();	
	// inicialitzacions camera axonometrica
	xleft = ybottom = -radi;
	xright = ytop = radi;
	// inicialitzacions camera perspectiva
	dist = 2 * radi;
	alpha_v = asin(radi / dist);
	fovy = 2 * (alpha_v * 180 / PI);
	aspect = 1;
}

void parametres_camera(int tipus)
{
	if (tipus == 0) {
		// parametres camera axonometrica
		zNear = 0;
		zFar = 2 * radi;
	} else if (tipus == 1) {
		// parametres camera perspectiva
		zNear = radi;
		zFar = 3 * radi;
	}
}

void initGL()
{
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(colours[0], colours[1], colours[2], colours[3]);

	tipus = visibilitat = vel = 1;
	zoom = walk = 0;
	inicialitzacions();
	parametres_camera(tipus);
	tipus_cam(tipus);
	pos_cam(tipus);
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
		glVertex3d(-5.0, 0.0, -5.0);
		glVertex3d(-5.0, 0.0, 5.0);
		glVertex3d(5.0, 0.0, 5.0);
		glVertex3d(5.0, 0.0, -5.0);
	glEnd();
	glPopMatrix();
}

void pinta_model(int num_model)
{
	glPushMatrix();
		double altura = ymax - ymin;
		double escala, rotate, movex, movez;
		escala = rotate = movex = movez = 0.0;
		if (num_model == 0) {
			escala = 1.0 / altura;
			if (dir == 0) rotate = -90;
			else if (dir == 1) rotate = 0;
			else if (dir == 2) rotate = 90;
			else if (dir == 3) rotate = 180;
		} else {
			escala = 1.5 / altura;
			movex = movez = 2.5;
		}

		if (modelx > escena_xmax - ((xmax - xmin) / 2.0) * escala)
			modelx = escena_xmax - ((xmax - xmin) / 2.0) * escala;
		if (modelx < escena_xmin + ((xmax - xmin) / 2.0) * escala)
			modelx = escena_xmin + ((xmax - xmin) / 2.0) * escala;
		if (modelz > escena_zmax - ((zmax - zmin) / 2.0) * escala)
			modelz = escena_zmax - ((zmax - zmin) / 2.0) * escala;
		if (modelz < escena_zmin + ((zmax - zmin) / 2.0) * escala)
			modelz = escena_zmin + ((zmax - zmin) / 2.0) * escala;

		// moviment del patricio
		if (num_model == 0) glTranslated(modelx, 0.0, modelz);

		// escalar, desplasar
		double aux_trans = (ymin - ((ymin + ymax) / 2.0)) * escala;
		glTranslated(movex, -aux_trans, movez);
		glRotated(rotate, 0, 1, 0);
		glScaled(escala, escala, escala);
		glTranslated(-(xmax + xmin) / 2.0, -(ymin + ymax) / 2.0, -(zmax + zmin) / 2.0);
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

void pinta_parets()
{
	if (visibilitat) {
		glPushMatrix();
			glTranslated(0.0, 0.75, -4.9);
			glScaled(50.0, 7.5, 1.0);
			glColor3d(0.23137255, 0.74901961, 0.41960784);
			glutSolidCube(0.2);
		glPopMatrix();
		glPushMatrix();
			glTranslated(1.5, 0.75, 2.5);
			glScaled(1.0, 7.5, 20.0);
			glColor3d(0.23137255, 0.74901961, 0.41960784);		
			glutSolidCube(0.2);
		glPopMatrix();
	}
}

void refresh()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
		pinta_terra();
		glPushMatrix();
			glTranslated(2.5, 0.4, -2.5);
			pinta_ninot();
		glPopMatrix();
		glPushMatrix();
			glTranslated(-2.5, 0.4, 2.5);
			pinta_ninot();
		glPopMatrix();
		glPushMatrix();
			glTranslated(-2.5, 0.4, -2.5);
			pinta_ninot();
		glPopMatrix();
		pinta_model(0);
		pinta_model(1);
		pinta_parets();
	glPopMatrix();
	glutSwapBuffers();
}

void resize(int w, int h)
{
	win_x = w;
	win_y = h;
	double ra_v = (double)w / (double)h;
	if (tipus == 0) {
		if (ra_v >= 1) {
			xleft = -radi * ra_v;
			xright = radi * ra_v;
		} else if (ra_v < 1) {
			ybottom = -radi / ra_v;
			ytop = radi / ra_v;
		}
	} else {
		if (ra_v < 1) {
			alpha_v = atan(tan(alpha_v) / ra_v);
			fovy = 2 * alpha_v;
		}
		aspect = ra_v;
	}
	tipus_cam(tipus);
	glViewport(0, 0, w, h);
}

void click(int a, int b, int x, int y)
{
	if (b == GLUT_DOWN)
	{
		aux_x = x;
		aux_y = y;
	}
	if (b == GLUT_UP)
	{
		aux_x = aux_y = 0;
	}
}

void move(int x, int y)
{
	if (state == 2) {
		// fa el mateix que la rotació pero rotant la camera en comptes de l'escena
		angley += 360 * (2 * (x - aux_x) / (double)win_x);
		anglex += 360 * (2 * (aux_y - y) / (double)win_y);
		pos_cam(tipus);
	} else if (state == 3) {
		double aux = 2 * (aux_y - y) / (double)win_y;
		if (tipus == 0) {
			xleft += aux;
			xright -= aux;
			ybottom += aux;
			ytop -= aux;
		} else {
			fovy += aux * 10;
		}
		tipus_cam(tipus);
	}
	aux_x = x;
	aux_y = y;
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
		cout << "Pitjeu 'e' per canviar els angles d'Euler\n";
		cout << "Pitjeu 'm' per canviar el model" << endl;
		cout << "Pitjeu 't' per moure el model, que es moura amb les tecles: a, s, d, w.\n";
		cout << "	Amb les tecles z i x s'agumenta o disminueix la velocitat de moviment\n";
		cout << "Pitjeu 'p' per canviar el tipus de camera" << endl;
		cout << "Pitjeu 'r' per fer un reset de la imatge" << endl;
		cout << "Pitjeu 'y' per fer zoom movent el ratoli" << endl;
		cout << "Pitjeu 'v' per fer les parets visibles o no" << endl;
		cout << "Pitjeu 'c' per canviar la camera a primera persona" << endl;
		cout << "Pitjeu 'i' per activar/desactivar la iluminacio" << endl;
		cout << "Pitjeu ESC per sortir de l'aplicacio.\n";
	}
	else if (a == 'i') {
		if (iluminacio) iluminacio = 0;
		else iluminacio = 1;
		initGL();
	}
	else if (a == 'p') {
		if (tipus == 0) tipus = 1;
		else tipus = 0;
		parametres_camera(tipus);
		tipus_cam(tipus);
		pos_cam(tipus);
	}
	else if (a == 'v') {
		if (visibilitat) visibilitat = 0;
		else visibilitat = 1;
	}
	else if (a == 'c') {
		if (walk) walk = 0;
		else walk = 1;
		tipus = 1;
		fovy = 60.0;
		zNear = 2.0;
		zFar = 8.0;
		tipus_cam(tipus);
		pos_cam(tipus);
	}
	else if (a == 'r') initGL();
	else if (a == 'y') state = 3;
	else if (a == 'e') state = 2;
	else if (a == 't') state = 1;
	else if (a == 27) exit(0);
	else if (a == 'm') {
		cout << "Introdueix nom del nou model: ";
		cin >> filename;
		calcCapsa();
	}
	else if (a == 'n') state = 0;

	// moviment del patricio central
	if (state == 1) {
		if (a == 'w') {
			if (dir == 0) modelx -= MOV * vel;
			else if (dir == 1) modelz += MOV * vel;
			else if (dir == 2) modelx += MOV * vel;
			else if (dir == 3) modelz -= MOV * vel;
		}
		if (a == 's') {
			if (dir == 0) modelx += MOV * vel;
			else if (dir == 1) modelz -= MOV * vel;
			else if (dir == 2) modelx -= MOV * vel;
			else if (dir == 3) modelz += MOV * vel;
		}
		if (a == 'd') {
			if (dir == 0) dir = 3;
			else --dir;
		}
		if (a == 'a') {
			if (dir == 3) dir = 0;
			else ++dir;
		}
		if (a == 'z') {
			if (vel <= 1) vel = 1;
			else --vel;
		}
		if (a == 'x') ++vel;
		if (walk) pos_cam(tipus);
	}

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
