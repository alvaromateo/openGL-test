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

vector<double> colours(4, 0.0);
vector<double> trans(3, 0.0);
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


/*  aux_x and aux_y help control to check if a mouse action is
    a click or a movement
*/
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

void refresh(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/*	Codi pel qual hi ha la pregunta del final
	glRotated(angle1, 0, 1, 0);
	glRotated(angle2, 1, 0, 0);
	if (!fi_move) glTranslated(trans2[0], trans2[1], trans2[2]);
	glTranslated(trans[0], trans[1], trans[2]);
	*/
	if (state == 0) {
		glTranslated(trans[0], trans[1], trans[2]);
	} else if (state == 2 && order == 0) {
		glRotated(angle1, 0, 1, 0);
		glRotated(angle2, 1, 0, 0);
	} else if (state == 2 && order == 1) {
		glRotated(angle2, 0, 0, 1);
		glRotated(angle1, 0, 1, 0);
	} else if (state == 3) {
		// si el objecte no esta al centre de coordenades a part de escalar-lo
		// tambe es mou una mica.
		glScaled(sca[0], sca[1], sca[2]);
	}

	glutWireTeapot(0.5);
	glLoadIdentity();
	glBegin(GL_LINES);
		//eix x
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(2.0, 0.0, 0.0);
		//eix y
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 2.0, 0.0);
		//eix z
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, 2.0);
	glEnd();

	glutSwapBuffers();
	
}

/* The parameters w and h gives us the size of the window in each moment */
void resize(int w, int h)
{
	win_x = w;
	win_y = h;

	GLint x_pos, y_pos, side;
	side = x_pos = y_pos = 0;
	if (w > h) {
		side = h;
		x_pos = (w - side) / 2;
	} else {
		side = w;
		y_pos = (h - side) / 2;
	}
	glViewport(x_pos, y_pos, side, side);
}

void move(int x, int y)
{
	if (state == 1) {
		if (colours[col] <= 0.2) {
			fade = 1;
		}
		if (colours[col] >= 1.0) {
			fade = 0;
		}

		if (fade) colours[col] += INC;
		else colours[col] -= INC;

		glClearColor(colours[0], colours[1], colours[2], 0);
	} else if (state == 2) {
		if (aux_x < x) {
			angle1 += ANGLE_INC;
			if (angle1 >= 360) angle1 = 0.0;
		} else if (aux_x > x) {
			angle1 -= ANGLE_INC;
			if (angle1 < 0) angle1 += 360.0;
		}
		if (aux_y < y) {
			angle2 += ANGLE_INC;
			if (angle2 >= 360) angle2 = 0.0;
		} else if (aux_y > y) {
			angle2 -= ANGLE_INC;
			if (angle2 < 0) angle2 += 360.0;
		}
	} else if (state == 0) {
		/* 	Regla de 3 per traslladar un moviment mesurat en pixels sobre la grandaria de la pantalla grafica
			a un numero sobre la grandaria de la camera (2, ja que va de -1 a 1).
			El numero a traslladar sobre l'eix de les y s'agafa restan d'aquesta manera perque les coordenades
			de glut situen l'origen a dalt a l'esquerra i les de opengl a baix a l'esquerra.
			(win_y - y) - (win_y - aux_y) = aux_y - y
		*/ 
		trans[0] += 2 * (x - aux_x) / (double)win_x;
		trans[1] += 2 * (aux_y - y) / (double)win_y;
	} else if (state == 3) {
		if (x > aux_x) {
			sca[0] += INC;
			sca[2] += INC;
		} else if (x < aux_x) {
			sca[0] -= INC;
			sca[2] -= INC;
		}

		if (y > aux_y) {
			sca[1] += INC;
			sca[2] += INC;
		} else if (y < aux_y) {
			sca[1] -= INC;
			sca[2] -= INC;
		}
	}
	aux_x = x;
	aux_y = y;
	glutPostRedisplay();
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

void key(unsigned char a, int x, int y)
{
	if (a == 'h') {
		cout << "Pitjeu 'f' per obrir el mode per canviar el color: \n";
		cout << "	Click amb el ratoli per canviar de color el fons\n";
		cout << "	Arrossegueu el ratoli per canviar la tonalitat de color\n";
		cout << "Pitjeu 'n' per tornar al mode normal (mode traslacio)\n";
		cout << "Pitjeu 'h' per mostrar l'ajuda\n";
		cout << "Pitjeu 'm' per modificar els angles girats de la tetera\n";
		cout << "	Arrossegar amb el ratoli en la direccio x+ per augmentar l'angle,\n";
		cout << "	en la direccio x- per disminuir-lo i el mateix per la direccio y+ i y-\n";
		cout << "Pitjeu 'r' per canviar l'ordre de les rotacions\n";
		cout << "Pitjeu 's' per escalar el model\n";
		cout << "	Arrosegueu el ratoli en la direccio x+ per augmentar l'objecte i en la\n";
		cout << "	direccio x- per disminuir-lo respecte l'eix x, i igual respecte el y\n";
		cout << "Pitjeu ESC per sortir de l'aplicacio.\n";
	}
	else if (a == 27) exit(0);
	else if (a == 'r') {
		if (order == 0) order = 1;
		else if (order == 1) order = 0;
	}
	else if (a == 's') state = 3;
	else if (a == 'm') state = 2;
	else if (a == 'f') state = 1;
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

/* Per que al rotar despres al traslladar es mou en direccio contraria al desplasament del mouse? */
