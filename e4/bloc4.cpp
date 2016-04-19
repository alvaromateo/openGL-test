#if defined(__APPLE__)
	#include <OpenGL/OpenGL.h>
	#include <GLUT/GLUT.h>
#else
	#include <GL/gl.h>
	#include <GL/freeglut.h>
	#include <GL/glu.h>
#endif

#include <iostream>
#include "model.h"
#include <cmath>

using namespace std;

int ancho, altura;


double rx=0.0; //rotacion tot
double ry=0.0; //rotacion tot

double tx=0.0; //trasllada tot
double ty=0.0; //trasllada tot

double e=1.0; //escalat tot


Model m;

int posInix;
int posIniy;

double xmax, xmin, ymax, ymin, zmax, zmin;

struct centreModel {
	double x,y,z;
};

struct dimensions {
	double x, y, z;
};

double maxDim; //dimensio maxima de la capsa

dimensions dim;
centreModel cm;

 
double radioEsfera=0;

double ra; //relacion de aspecto


//angulos euler
double anglex;
double angley;
double anglez;

//VRP
struct VRP {
	double x,y,z;
};

VRP vrp;


bool camara=false;
bool verPared=true; 

//ortogonal
double izquierda, derecha, arriba, abajo;

//trasladar patricio
double tpx=0;
double tpz=0;

//direccion patricio
double direccion=1; //direccion podra tener 4 valores: 1 sera x negativas, 2 sera z positivas, 3 sera x positivas y 4 sera z negativas

//angulo de giro de patricio
double giro=-90;

//velocidad de patricio
double velocidad=0.05;

double zoom=0.025;

bool boto_clicat=false;

//activa/descativa la iluminacio
bool ilum=false; //NUEVO


bool ocultar_esfera=false;

bool normal_cara=false;

bool luz1=true;
bool luz0=false;
bool luz2=true;

int posLuz=1;

float pos[]={radioEsfera, 1.5, -radioEsfera, 1.0};

//glut pone la figura en el centro que es el 0,0

void calcular_caixa(Model m2) {
	double x,y,z;
	xmax =  xmin = m2.vertices()[0]; 
    ymax =  ymin = m2.vertices()[1];
    zmax =  zmin = m2.vertices()[2];
	for (int i=3; i < m2.vertices().size(); ++i) {
		if (i%3==0) { // x
			x=m2.vertices()[i]; 
			if (x > xmax) xmax=x;
			else if (x<xmin) xmin=x;
		}
		if (i%3==1) { // y
			y=m2.vertices()[i];
			if (y> ymax) ymax=y;
			else if (y<ymin) ymin=y;
		}
		if (i%3==2) { // z
			z=m2.vertices()[i];
			if (z> zmax) zmax=z;
			else if (z<zmin) zmin=z;
		}
		
    }
	
	cm.x=(xmax+xmin)/2;
	cm.y=(ymax+ymin)/2;
	cm.z=(zmax+zmin)/2;

	dim.x=xmax-xmin;
	dim.y=ymax-ymin;
	dim.z=zmax-zmin;
	
	if (dim.x>dim.y) maxDim=dim.x;
	else maxDim=dim.y;
	if (dim.z>maxDim) maxDim=dim.z;
}

void pintaModel(Model m2) {
	glEnable(GL_DEPTH_TEST);
    	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_FRONT_AND_BACK: pinta el modelo por delante y por atras. GL_FILL: pinta todo el muñeco y no solo alambres
	int anterior=-1;
    	for (int i=0; i < m2.faces().size(); ++i) { //el modelo esta formado por caras que son triangulos. Se lee cada triangulo y se pinta
    		const Face & f = m2.faces()[i];
    		glBegin(GL_TRIANGLES);
			int material=m2.faces()[i].mat; //posicion del vector Materials que contiene todos los colores de esa cara, es decir, todas las componentes de esa cara
			if (material!=anterior) {
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   Materials[material].diffuse);
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   Materials[material].ambient);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  Materials[material].specular);
				glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, Materials[material].shininess);
			}
			if (normal_cara) if (f.n.size()==0) glNormal3f(m.faces()[i].normalC[0], m.faces()[i].normalC[1], m.faces()[i].normalC[2]);
			if (f.n.size()!=0 and normal_cara==false) glNormal3dv(&m2.normals()[f.n[0]]);
        		glVertex3dv(&m2.vertices()[f.v[0]]); //coge los vertices del triangulo
			if (f.n.size()!=0 and normal_cara==false) glNormal3dv(&m2.normals()[f.n[0]]);
        		glVertex3dv(&m2.vertices()[f.v[1]]);
			if (f.n.size()!=0 and normal_cara==false) glNormal3dv(&m2.normals()[f.n[0]]);
        		glVertex3dv(&m2.vertices()[f.v[2]]);
    		glEnd();
	anterior=material;
    }
}

void calculaEsfera() {
	
	double maxXEsfera=5.0; // punto maximo del suelo
	
	double maxYEsfera=1.5;  // punto maximo de patricio
	
	double maxZEsfera=5.0; // punto maximo del suelo
	
	//calcular la distancia entre los maximos y el 0 porque el radio de la esfera empezara en 0, es decir, en el centro
	maxXEsfera=pow((maxXEsfera-0), 2);
	maxYEsfera=pow((maxYEsfera-0), 2);
	maxZEsfera=pow((maxZEsfera-0), 2);
	
	radioEsfera = sqrt(maxXEsfera+maxYEsfera+maxZEsfera);

	izquierda=radioEsfera;
	derecha=radioEsfera;
	arriba=radioEsfera;
	abajo=radioEsfera;

}

void ninot_de_neu() {
	glPushMatrix();

	GLfloat mat_blanco[]={1.0,1.0,1.0,1.0};
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_blanco);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,mat_blanco);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  mat_blanco);
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100);
	glutSolidSphere(0.4,16,16); //cuerpo
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(0,0.6,0);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_blanco);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,mat_blanco);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  mat_blanco);
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100);
	glutSolidSphere(0.2,16,16); //cabeza
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.1,0.6,0);
	glRotated(90,0.0,1.0,0.0);
	GLfloat mat_naranja[]={1.0,0.5,0.15,1.0};
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_naranja);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,mat_naranja);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  mat_naranja);
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100);
	glutSolidCone(0.1,0.2,16,16); //nariz	
	glPopMatrix();
}
//color ambiental y difusa siempre igual
void suelo() {
	glPushMatrix();
	//glScaled (0.75,0.75,0.75);
	glBegin(GL_QUADS);          //suelo
		glNormal3d(0.0,1.0,0.0);
		GLfloat mat_azul[]={0.0,0.0,1.0,1.0}; //azul Ultimo parametro es el alpha: 1 si no es transparente y 0 transparente
		GLfloat mat_blanco[]={1.0,1.0,1.0,1.0};
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_blanco);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,mat_azul);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  mat_azul);
		glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100);
		
		glVertex3f(-5.0,0.0,5.0);
		glVertex3f(5.0,0.0,5.0);
		glVertex3f(5.0,0.0,-5.0);
		glVertex3f(-5.0,0.0,-5.0);
	glEnd(); 
	glPopMatrix();
}

void euler() {
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	vrp.x=0;
	vrp.y=0;
	vrp.z=2;
	glTranslated(0.0,0.0,-2*radioEsfera);
	glRotated(-anglez,0.0,0.0,1.0);
	glRotated(anglex,1.0,0.0,0.0);
	glRotated(-angley,0.0,1.0,0.0);
	glTranslated(-vrp.x,-vrp.y,-vrp.z);
	glutPostRedisplay();
}

void refresh(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	
 	
	//hay que trasladar todo el modelo para que se vea la esfera y hay que trasladarlo la distancia de una esfera (2*radio)
	
	glTranslated(0.0,0.0,-2*radioEsfera);
	glRotated(rx,1.0,0.0,0.0);
	glRotated(ry,0.0,1.0,0.0); //girar la imagen
	glScaled (e,e,e); //escalar toda la imagen
glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glTranslated(tx,ty,0.0);	//mover toda la imagen	
	
	glPushMatrix();
	if (luz2==false) {
		pos[0]=tpx;
		pos[1]=2.0;
		pos[2]=tpz;
		pos[3]=1.0;

		glLightfv(GL_LIGHT2, GL_POSITION, pos);
	}
	glTranslated(tpx,0.0,tpz);
	glRotated(giro,0.0,1.0,0.0);
	//glRotated(-90,0.0,1.0,0.0);
	glTranslated(0.0,(1/maxDim)*dim.y/2,0.0);
	glScaled (1/maxDim,1/maxDim,1/maxDim); 
	glTranslated(-cm.x,-cm.y,-cm.z); 
	pintaModel(m); //patricio 1
	glPopMatrix();	

	glPushMatrix();
	glTranslated(2.5,(1.5/maxDim)*dim.y/2,2.5);
	glScaled (1.5/maxDim,1.5/maxDim,1.5/maxDim); 
	glTranslated(-cm.x,-cm.y,-cm.z);
	pintaModel(m); //patricio 2
	glPopMatrix();

	glPushMatrix();
	glTranslated(-2.5,0.4,-2.5);
	ninot_de_neu();	//muñeco
	glPopMatrix();

	glPushMatrix();
	glTranslated(-2.5,0.4,2.5);
	ninot_de_neu();	//muñeco
	glPopMatrix();	

	glPushMatrix();
	glTranslated(2.5,0.4,-2.5);
	ninot_de_neu();	//muñeco
	glPopMatrix();

	glPushMatrix();
	suelo();        //suelo
	glPopMatrix();	


	if (verPared==true) {
		glPushMatrix(); 
		glTranslated(0,0.75,-4.9);
		glScaled(10,1.5,0.2);
		GLfloat mat_negro[]={0.0,0.0,0.0,1.0}; //verde
		GLfloat mat_verde[]={0.0,1.0,0.0,1.0}; //Ultimo parametro es el alpha: 1 si no es transparente y 0 transparente
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_negro); //especular es el color del brillo
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,mat_negro); //ambiente forma en que el objeto refleja la luz del objeto
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  mat_verde); //color del objeto
		glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0); //define la grandaria del brillo. Si el numero es alto el brillo estara mas concentrado
		glutSolidCube(1); //pared
		glPopMatrix();

		glPushMatrix(); 
		glTranslated(1.5,0.75,2.5);
		glScaled(0.2,1.5,4);
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_negro); //especular es el color del brillo
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,mat_negro); //ambiente forma en que el objeto refleja la luz del objeto
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  mat_verde); //color del objeto
		glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0); //define la grandaria del brillo. Si el numero es alto el brillo estara mas concentrado
		glutSolidCube(1); //pared
		glPopMatrix();
	}
	if (ocultar_esfera==true) {
		glPushMatrix();
		glutWireSphere(radioEsfera,16,16); //esfera
		glPopMatrix();	
	}
	glutSwapBuffers();
}

void tipusCamera() {
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	cout << camara << endl;
	if (camara==true) {
	cout << 11 << endl;
		//glOrtho(limiteIzquierdo, limiteDerecho, limiteAbajo, limiteArriba, znear, zfar)
		glOrtho (-izquierda,derecha,-abajo,arriba,radioEsfera,3*radioEsfera);
		//glOrtho (-radioEsfera,radioEsfera,-radioEsfera,radioEsfera,radioEsfera,3*radioEsfera);
		camara=false;
	}
	else {
		//glPerspective(fovy, ra, znear, zfar);
		
		gluPerspective(60, ra, radioEsfera,3*radioEsfera);
		camara=true;
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}

//si han clicat: GLUT_DOWN
void clic(int boto, int estat_buto, int x, int y) {
	if (boto==GLUT_RIGHT_BUTTON and estat_buto==GLUT_DOWN) boto_clicat=true;
	else if (boto==GLUT_RIGHT_BUTTON and estat_buto==GLUT_UP) boto_clicat=false;
	posInix=x;
	posIniy=y;
}

void rotar(int x, int y) {
	if (x<posInix) ry=ry-5.0;

	else if (x>posInix) ry=ry+5.0;

	else if (y<posIniy) rx=rx-5.0;

	else if (y>posIniy) rx=rx+5.0;
	posInix=x;
	posIniy=y;
	glutPostRedisplay();

}

void trasllada(int x, int y) {
	if (x<posInix and tx>=-1) tx=tx-0.05;

	else if (x>posInix and tx<=1) tx=tx+0.05;

	else if (y<posIniy and ty<=1) ty=ty+0.05;

	else if (y>posIniy and ty>=-1) ty=ty-0.05;
	posInix=x;
	posIniy=y;
	glutPostRedisplay();
}

void escalar(int x, int y) {
	//if (x<posInix and tx>=-1) tx=tx-0.05;

	//else if (x>posInix and tx<=1) tx=tx+0.05;

	if (y<posIniy) e=e+0.01;

	else if (y>posIniy) e=e-0.01;
	posInix=x;
	posIniy=y;
	glutPostRedisplay();
}

//tecla==4 alt

void gestio_mouse (int x, int y) {
	if (boto_clicat) {
		if (y>posIniy) { //arriba
			izquierda+=zoom;
			derecha+=zoom;
			arriba+=zoom;
			abajo+=zoom;
		}
		else if (y<posIniy) { //abajo
			izquierda-=zoom;
			derecha-=zoom;
			arriba-=zoom;
			abajo-=zoom;
		}
		tipusCamera();
		cout << izquierda <<endl;
	}
	int tecla=glutGetModifiers(); //indica la tecla que se esta presionando
	if (tecla==1) rotar(x,y); //shift
	else if (tecla==2) escalar(x,y); //ctrl
	posInix=x;
	posIniy=y;
}

void avanza_patricio() {
	if (direccion==1) tpx=tpx-velocidad; //patricio esta mirando hacia las x negativas
	else if (direccion==2) tpz=tpz+velocidad; //patricio esta mirando hacia las z positivas 
	else if (direccion==3) tpx=tpx+velocidad; //patricio esta mirando hacia las x positivas 
	else if (direccion==4) tpz=tpz-velocidad; //patricio esta mirando hacia las z negativas 	
	glutPostRedisplay();
}

void retrocede_patricio() {
	if (direccion==1) tpx=tpx+velocidad; //patricio esta mirando hacia las x negativas
	else if (direccion==2) tpz=tpz-velocidad; //patricio esta mirando hacia las z positivas 
	else if (direccion==3) tpx=tpx-velocidad; //patricio esta mirando hacia las x positivas 
	else if (direccion==4) tpz=tpz+velocidad; //patricio esta mirando hacia las z negativas 
	glutPostRedisplay();
}

void mover_izquierda_patricio() {
	if (giro==360) giro=0;	
	giro=giro+90;
	if (direccion==1) direccion=2;
	else if (direccion==2) direccion=3;
	else if (direccion==3) direccion=4;
	else if (direccion==4) direccion=1;
	glutPostRedisplay();
}

void mover_derecha_patricio() {
	if (giro==0) giro=360;	
	giro=giro-90;	
	if (direccion==1) direccion=4;
	else if (direccion==4) direccion=3;
	else if (direccion==3) direccion=2;
	else if (direccion==2) direccion=1;
	glutPostRedisplay();
}

void incrementar_velocidad_patricio() {
	velocidad+=0.05;
}

void decrementar_velocidad_patricio() {
	velocidad-=0.05;
}

void mostrarPared() {
	if (verPared==true) verPared=false;
	else verPared=true;
	glutPostRedisplay();
}

void mostrar_esfera() {
	if (ocultar_esfera==true) ocultar_esfera=false;
	else ocultar_esfera=true;
	glutPostRedisplay();
}

void canviar_normal() {
	if (normal_cara==true) normal_cara=false;
	else normal_cara=true;
	glutPostRedisplay();
}

void cambiar_luz1() {
	if (luz1) {
		cout << "activar luz1: " << luz1 << endl;
		glEnable(GL_LIGHT1); //encen una llum per defecte
		
		GLfloat blanco[]={1.0,1.0,1.0,1.0}; //blanco
		GLfloat gris[]={0.5,0.5,0.5,1.0}; //gris	

		glLightfv(GL_LIGHT1, GL_POSITION, pos);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, gris);
		glLightfv(GL_LIGHT1, GL_AMBIENT, gris);
		glLightfv(GL_LIGHT1, GL_SPECULAR, blanco);
		luz1=false;
	}
	else {
		cout << "desactivar luz1: " << luz1 << endl;
		glDisable(GL_LIGHT1);
		luz1=true;
	}
	glutPostRedisplay();
}

void cambiar_luz0() {
	if (luz0) {
		cout << "activar luz0: " << luz0 << endl;
		glEnable(GL_LIGHT0); //encen una llum per defecte

		GLfloat blanco[]={1.0,1.0,1.0,1.0}; //blanco
		GLfloat gris[]={0.5,0.5,0.5,1.0}; //gris	

		glLightfv(GL_LIGHT0, GL_POSITION, pos);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, gris);
		glLightfv(GL_LIGHT0, GL_AMBIENT, gris);
		glLightfv(GL_LIGHT0, GL_SPECULAR, blanco);
		luz0=false;
	}
	else {
		cout << "desactivar luz0: " << luz0 << endl;
		glDisable(GL_LIGHT0);
		luz0=true;
	}
	glutPostRedisplay();
}

void cambiar_luz2() {
	if (luz2) {
		cout << "activar luz2: " << luz2 << endl;
		glEnable(GL_LIGHT2); //encen una llum per defecte

		GLfloat blanco[]={1.0,1.0,1.0,1.0}; //blanco
		GLfloat gris[]={0.5,0.5,0.5,1.0}; //gris	

		glLightfv(GL_LIGHT2, GL_POSITION, pos);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, gris);
		glLightfv(GL_LIGHT2, GL_AMBIENT, gris);
		glLightfv(GL_LIGHT2, GL_SPECULAR, blanco);
		luz2=false;

		//glTranslated(0.0,(1/maxDim)*dim.y/2,0.0);
		pos[0]=0.0;
		pos[1]=1.0;
		pos[2]=0.0;
		pos[3]=1.0;

	}
	else {
		cout << "desactivar luz2: " << luz2 << endl;
		glDisable(GL_LIGHT2);
		luz2=true;
	}
	glutPostRedisplay();
}

void reset() {
	rx=0.0; //rotacion tot
	ry=0.0; //rotacion tot
	tx=0.0; //trasllada tot
	ty=0.0; //trasllada tot
	tpx=0.0; //trasllada patricio
	tpz=0.0; //trasllada patricio
	e=1.0; //escalat tot
	direccion=1;
	giro=0;
	velocidad=0.05;
	boto_clicat=false;
	camara=false;
	normal_cara=false;
	ilum=false; //NUEVO
	ocultar_esfera=false;
	luz1=true;
	luz0=false;
	luz2=true;
	izquierda=radioEsfera;
	derecha=radioEsfera;
	arriba=radioEsfera;
	abajo=radioEsfera;
	tipusCamera();
}

void iluminacio() { //NUEVO
	if (ilum) {
		cout << "activar iluminacion: " << ilum << endl;
		glEnable(GL_LIGHTING); //activa la iliminacio
		ilum=false;
	}
	else {
		cout << "desactivar iluminacion: " << ilum << endl;
		glDisable(GL_LIGHTING);
		ilum=true;
	}
}

void cambiar_pos_luz() {
	double x,y,z;
	double aux=5; //el suelo esta situado en el centro (0,0,0) por lo que la esquina derecha esta en el (5,0,5), la izquierda en el (-5,0,5), la izquierda arriba en el (-5,0,-5) y derecha arriba (5,0,-5)
	y=1.5;
	if (posLuz==1) { //esquina derecha abajo
		x=aux;
		z=aux;
	}
	else if (posLuz==2) { //esquina izquierda abajo
		x=-aux;
		z=aux;
	}
	else if (posLuz==3) { //esquina izquierda arriba
		x=-aux;
		z=-aux;
	}
	else if (posLuz==4) { //esquina derecha arriba
		x=aux;
		z=(-aux);
	}
	else if (posLuz==5) { //patricio
//glTranslated(2.5,(1.5/maxDim)*dim.y/2,2.5);
		x=2.5;
		y=1.5;
		z=2.5;
	}
	pos[0]=x;
	pos[1]=y;
	pos[2]=z;
	pos[3]=1.0;
	++posLuz;
	if (posLuz==6) posLuz=1;
	glutPostRedisplay();
}

void teclat (unsigned char tecla, int x, int y) {
	if (tecla=='h') {
		cout << "Opciones:" << endl;
		cout << "Shift: Rotar imagen" << endl;
		cout << "Ctrl: Escalar imagen" << endl;
		cout << "t: Trasladar imagen" << endl;
		cout << "c: Moure legoman" << endl;
		cout << "h: Mostrar ayuda" << endl;
		cout << "p: Canviar el tipus de camera" << endl;
		cout << "r: Fer reset" << endl;
		cout << "e: Implementar euler" << endl;
		cout << "v: Mostrar/ocultar pareds" << endl;
		cout << "w: Avança Patricio" << endl;
		cout << "s: Retrocedeix Patricio" << endl;
		cout << "a: Moure a l'esquerra el Patricio" << endl;
		cout << "d: Moure a la dreta el Patricio" << endl;
		cout << "z: Incrementar la velocitat del Patricio" << endl;
		cout << "x: Decrementar la velocitat del Patricio" << endl;
		cout << "i: Activar/Desactivar iluminacio" << endl; //NUEVO
		cout << "d: Mostrar/Ocultar esfera" << endl;
		cout << "n: Canviar normal de cara/vertice" << endl;
		cout << "0: Activar/desactivar LIGHT0" << endl;
		cout << "1: Activar/desactivar LIGHT1" << endl;
		cout << "2: Activar/desactivar LIGHT2" << endl;
		cout << "m: Cambiar la posicion de la luz" << endl;
		cout << "ESC: Finalizar programa" << endl;
	}
	else if (tecla==27) exit(0);
	else if (tecla=='t') trasllada(x,y);
	else if (tecla=='p') tipusCamera();
	else if (tecla=='r') reset();
	else if (tecla=='e') euler();
	else if (tecla=='v') mostrarPared();
	else if (tecla=='w') avanza_patricio();
	else if (tecla=='s') retrocede_patricio();
	else if (tecla=='a') mover_izquierda_patricio();
	else if (tecla=='d') mover_derecha_patricio();
	else if (tecla=='z') incrementar_velocidad_patricio();
	else if (tecla=='x') decrementar_velocidad_patricio();
	else if (tecla=='i') iluminacio(); //NUEVO
	else if (tecla=='o') mostrar_esfera();
	else if (tecla=='n') canviar_normal();
	else if (tecla=='1') cambiar_luz1();
	else if (tecla=='0') cambiar_luz0();
	else if (tecla=='2') cambiar_luz2();
	else if (tecla=='m') cambiar_pos_luz();

}

void centraFigura (int ample, int alt) {
	ra=(double)ample/(double)alt;
	if (ra>=1.0) { //si ra>1 significa que el ancho es mayor que la altura por lo tanto hay que ampliar el radio de la esfera para que se haga mas grande y por eso se multiplica por ra
		izquierda=radioEsfera*ra;
		derecha=radioEsfera*ra; 

	}
	else { //si ra<1 significa que la altura es mayor a la anchura y hay que hacer el radio mas pequeño
		arriba=radioEsfera/ra;
		abajo=radioEsfera/ra; 
		
	}
	glViewport(0,0,ample,alt);
	tipusCamera();

}

int main (int argc, const char * argv[])
{
	
    	m.load("Patricio.obj"); //carga el modelo
	calcular_caixa(m);
	calculaEsfera();
	//glEnable(GL_DEPTH_TEST);


	

	glutInit (&argc, (char **)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600,600);
	
	glutCreateWindow("IDI: Practiques OpenGL");

	glEnable(GL_NORMALIZE); //indica a opengl que antes de que utilice un vector lo normalice
	glEnable(GL_LIGHTING); //activa la iliminacio
	glEnable(GL_LIGHT0); //encen una llum per defecte
	


	GLfloat blanco[]={1.0,1.0,1.0,1.0}; //blanco
	GLfloat gris[]={0.5,0.5,0.5,1.0}; //gris	

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, gris);
	glLightfv(GL_LIGHT0, GL_AMBIENT, gris);
	glLightfv(GL_LIGHT0, GL_SPECULAR, blanco);

	glutDisplayFunc (refresh); //pinta objetos en pantalla
	

	glutReshapeFunc (centraFigura);

	glutMouseFunc (clic); //clicar
	glutMotionFunc (gestio_mouse); //arrastrar
	glutKeyboardFunc (teclat);

	glutMainLoop();
	return 0;
}
