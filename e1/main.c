#include <iostream>
#include <vector>

#if defined(__APPLE__)
	#include <OpenGL/OpenGL.h>
	#include <GLUT/glut.h>
#else
	#include <GL/gl.h>
	#include <GL/freeglut.h>
#endif

#define INC 0.01


void initGL();
void refresh(void);
void resize(int w, int h);
void move(int x, int y);
void click(int a, int b, int x, int y);
void key(unsigned char a, int x, int y);

std::vector<double> colours(3, 0.0);
std::vector< std::vector<double> > verts(3, std::vector<double>(3, 0.0));

/*  state = 0 --> no fa res al clicar amb el ratoli
	state = 1 --> canvia els colors
	state = 2 --> entra noves coordenades pel triangle
*/
int state = 0;
// 	next_v controls the input of the next vertex
int next_v = 0;

// 	col controls the current colour to be modified
int col = 1;
// 	fade controls if the colour must get brighter or darker
int fade = 0;

/*  aux_x and aux_y help control to check if a mouse action is
    a click or a movement
*/
int aux_x = 0;
int aux_y = 0;


void initGL()
{
	// Inicialització v1
	verts[0][0] = -0.5;
	verts[0][1] = -0.3333;
	verts[0][2] = 0;
	// Inicialització v2
	verts[1][0] = 0.5;
	verts[1][1] = -0.3333;
	verts[1][2] = 0;
	// Inicialització v3
	verts[2][0] = 0;
	verts[2][1] = 0.6666;
	verts[2][2] = 0;

	colours[1] = 0.50;
	glClearColor(colours[0], colours[1], colours[2], 0);
}

void refresh(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	double* v1 = &verts[0][0];
	double* v2 = &verts[1][0];
	double* v3 = &verts[2][0];

	glBegin(GL_TRIANGLES);
		glColor3d(0.49803922, 1, 0.83137255);
		glVertex3dv(v1);
		glColor3d(0.0, 0.74901961, 1);
		glVertex3dv(v2);
		glColor3d(0.58039216, 0.0, 0.82745098);
		glVertex3dv(v3);
	glEnd();
	glutSwapBuffers();
}

/* The parameters w and h gives us the size of the window in each moment */
void resize(int w, int h)
{
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
		glutPostRedisplay();
	}
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
		} else if (state == 2) {
			int gl_coord_y, window_height, window_width;
			window_width = glutGet(GLUT_WINDOW_WIDTH);
			window_height = glutGet(GLUT_WINDOW_HEIGHT);
			gl_coord_y = window_height - y;

			verts[next_v][0] = (((double)x / (double)window_width) * 2) - 1;
			verts[next_v][1] = (((double)gl_coord_y / (double)window_height) * 2) - 1;
			if (next_v == 2) {
				next_v = 0;
				glutPostRedisplay();
			} else next_v++;
		}
		aux_x = aux_y = 0;
	}
}

void key(unsigned char a, int x, int y)
{
	if (a == 'h') {
		std::cout << "Pitjeu 'f' per obrir el mode per canviar el color: \n";
		std::cout << "	Click amb el ratoli per canviar de color el fons\n";
		std::cout << "	Arrossegueu el ratoli per canviar la tonalitat de color\n";
		std::cout << "Pitjeu 't' per poder introduir tres noves coordenades per dibuixar el triangle\n";
		std::cout << "Pitjeu 'n' per tornar al mode normal (interaccions no permeses)\n";
		std::cout << "Pitjeu 'h' per mostrar l'ajuda\n";
		std::cout << "Pitjeu ESC per sortir de l'aplicacio.\n";
	}
	else if (a == 27) exit(0);
	else if (a == 'f') state = 1;
	else if (a == 't') state = 2;
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

