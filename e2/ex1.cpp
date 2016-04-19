#if defined(__APPLE__)
	#include <OpenGL/OpenGL.h>
	#include <GLUT/glut.h>
#else
	#include <GL/gl.h>
	#include <GL/freeglut.h>
#endif

#include <iostream>
#include <vector>
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

// angle1, angle2 control the angle sto rotate the image
double angle1 = 0;
double angle2 = 0;

// win_x, win_y control the size of the window in pixels
int win_x = 600;
int win_y = 600;


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
	glutWireSphere(0.4, 20, 20);

	glPushMatrix();
	glTranslated(0.0, 0.6, 0.0);
	glutWireSphere(0.2, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.1, 0.6, 0);
	glRotated(90, 0, 1, 0);
	glutWireCone(0.1, 0.2, 20, 20);
	glPopMatrix();
}

/* Com fer perquè al escalar el ninot el cap i nas no es moguin? */
void refresh()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glRotated(angle1, 0, 1, 0);
	glRotated(angle2, 1, 0, 0);
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
		angle1 += 360 * (2 * (x - aux_x) / (double)win_x);
		angle2 += 360 * (2 * (aux_y - y) / (double)win_y);
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

void key(unsigned char a, int x, int y)
{
	if (a == 'h') {
		cout << "Pitjeu 'n' per tornar al mode normal\n";
		cout << "Pitjeu 'h' per mostrar l'ajuda\n";
		cout << "Pitjeu 'c' per obrir el mode per canviar el color\n";
		cout << "Pitjeu 'r' per modificar els angles girats de l'escena\n";
		cout << "Pitjeu 's' per escalar el model\n";
		cout << "Pitjeu ESC per sortir de l'aplicacio.\n";
	}
	else if (a == 27) exit(0);
	else if (a == 's') state = 3;
	else if (a == 'r') state = 2;
	else if (a == 'c') state = 1;
	else if (a == 'n') state = 0;
	glutPostRedisplay();
}

int main(int argc, const char *argv[])
{
	glutInit(&argc, (char **)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutCreateWindow("IDI: Practiques OpenGL");
	initGL();
	glutDisplayFunc(refresh);
	glutMouseFunc(click);
	glutMotionFunc(move);
	glutReshapeFunc(resize);
	glutKeyboardFunc(key);
	glutMainLoop();
	return 0;
}
