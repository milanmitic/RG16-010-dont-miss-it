#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#define TIMER_ID 0
#define TIME_INTERVAL 50

/* Deklaracije callback funkcija */
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int hight);
static void on_display(void);
static void on_timer(int value);

/* Dimenzije objekta */
const static float rect_x = 2.3;
const static float rect_y = 0.1;

static float x_curr, y_curr;  /* Tekuce koordinate centra objekta */	
static float v_x, v_y;		  /* Komponente vektora brzine kretanja objekta */
static int animaton_ongoing;  /* Fleg: 1 - animacija je u toku, inace nije */

/* Dimenzije prozora */
static int window_width, window_height;

int main(int argc,char* argv[])
{
	/* Inicijalizuje se GLUT */
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH |  GLUT_DOUBLE);

	/* Kreira se prozor */
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("DontMissIt");

	/* Registruju se funkcije za obradu dogadjaja */
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	glutDisplayFunc(on_display);

	x_curr = 0.0;
	y_curr = 2.0 - rect_y/2;
	v_y = -rect_y/2;
	animaton_ongoing=0;

	/* OpenGL inicijalizacija */
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);

	/* Ulazi se u glavnu petlju */
	glutMainLoop();

	return 0;
}

static void on_keyboard(unsigned char key, int x, int y)
{
	/* U zavisnosti od pritisnutog tastera izvrsava se odgovarajuca akcija */
	switch(key)
	{
		case 27:
			/* Zavrsava se program */
			exit(0);
			break;
		case 'g':
			/* Pokrece se animacija */
			if(!animaton_ongoing)
			{
				glutTimerFunc(TIME_INTERVAL, on_timer, TIMER_ID);
				animaton_ongoing = 1;
			}
			break;
		case 's':
			/* Zaustavlja se animacija */
			animaton_ongoing = 0;
			break;
		case 'q':
			/* Takodje se zaustavlja animacija ali samo ako se pritisne u 
			 * odgovarajucem trenutku
			 */
			if(y_curr <= -0.3 && y_curr >= -0.5)
				{
					animaton_ongoing = 0;
					printf("q\n");
				}
			break;
	}
}

static void on_timer(int value)
{
	/* Proverava se da li callback dolazi od odgovarajuceg tajmera */
	if(value != TIMER_ID)
		return;

	/* Azuriraju se koordinate centra objekta */
	x_curr += v_x;
	y_curr += v_y;
	printf("x_curr = %.2f, y_curr = %.2f\n",x_curr,y_curr);

	/* Forsira se ponovno iscrtavanje prozora */
	glutPostRedisplay();

	/* Po potrebi se ponovo postavlja tajmer */
	if(animaton_ongoing)
	{
		glutTimerFunc(TIME_INTERVAL, on_timer, TIMER_ID);
	}
}

static void on_reshape(int width, int height)
{
	window_width = width;
	window_height = height;
}

static void on_display(void)
{
	/* Postavlja se boja na odgovarajucu */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Postavlja se viewport */
	glViewport(0, 0, window_width, window_height);

	/* Postavljaju se parametri projekcije */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, window_width/(float)window_height, 1, 5);

	/* Postavlja se vidna tacka */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, -2, 3, 0, 0.6, 0, 0, 1.0, 0);

	/* Crtamo osnovnu traku */
	glBegin(GL_QUADS);
 	glColor3f(0.3, 1.0, 1.0);     
 	glVertex3f(-1.2, -1.0, 0);
 	glVertex3f(-1.2, 4.0, 0);
 	glVertex3f(1.2, 4.0, 0);
 	glVertex3f(1.2, -1.0, 0);
	glEnd();

	/* Crtamo objekat */
    glColor3f(0.2,0.6,1);
    glBegin(GL_POLYGON);
        glVertex3f(x_curr - rect_x / 2, y_curr - rect_y / 2, 0.11);
        glVertex3f(x_curr + rect_x / 2, y_curr - rect_y / 2, 0.11);
        glVertex3f(x_curr + rect_x / 2, y_curr + rect_y / 2, 0.11);
        glVertex3f(x_curr - rect_x / 2, y_curr + rect_y / 2, 0.11);
    glEnd();

    /* Postavlja se nova slika na prozor */
	glutSwapBuffers();
}