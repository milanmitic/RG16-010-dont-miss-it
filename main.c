#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>

#include "draw.h"

#define SUMMON_LINE_TIMER 0
#define REFRESH_TIMER 1
#define SUMMON_LINE_OBJECTS_TIMER 2
#define OBJECTS_REFRESH_TIMER 3

#define TIME_INTERVAL 1000
#define REFRESH_INTERVAL 3

/* Deklaracije callback funkcija */
static void on_keyboard(unsigned char key, int x, int y);
static void keyboard_up(unsigned char key, int x, int y);
static void on_reshape(int width, int hight);
static void on_display(void);
static void on_timer(int value);

static float v_x, v_y;	/* Komponente vektora brzine kretanja objekta */
static int animaton_ongoing;  /* Fleg: 1 - animacija je u toku, inace nije */

/* Dimenzije prozora */
static int window_width, window_height;

extern float rect_y;
extern float block_x;
extern float block_y;
extern line lines[];
extern line_object line_objects[];
extern int next;
extern float angle;
extern int random_interval;
extern int indeks;

// Treba promeniti
int key_combination[256];


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
	glutKeyboardUpFunc(keyboard_up);

	v_y = -rect_y/2 - 0.02; 
	animaton_ongoing = 0;
	srand(time(NULL));

	/* OpenGL inicijalizacija */
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);

	/* Postavljaju se pocetne vrednosti, 
	 	da se ne iscrtava na traci pre pokretanja animacije */
	int k;	
	for(k=0; k<NUMBER_OF_OBJECTS; k++)
	{
		lines[k].x_curr = 0.0;
		lines[k].y_curr = -3.0;

		line_objects[k].x_q = -3.0;
		line_objects[k].x_w = -3.0;
		line_objects[k].x_e = -3.0;
		line_objects[k].x_r = -3.0;
		line_objects[k].x_t = -3.0;
		//line_objects[k].y_curr = -3.0;
	}

	/* Ulazi se u glavnu petlju */
	glutMainLoop();

	return 0;
}

static void keyboard_up(unsigned char key, int x, int y)
{
	key_combination['q'] = 0;
	key_combination['w'] = 0;
	key_combination['e'] = 0;
	key_combination['r'] = 0;
	key_combination['t'] = 0;
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
				glutTimerFunc(TIME_INTERVAL, on_timer, SUMMON_LINE_TIMER);
				glutTimerFunc(REFRESH_TIMER, on_timer, REFRESH_TIMER);

				glutTimerFunc(2000, on_timer, SUMMON_LINE_OBJECTS_TIMER);
				glutTimerFunc(REFRESH_TIMER, on_timer, OBJECTS_REFRESH_TIMER);
				
				animaton_ongoing = 1;
			}
			break;
		case 'q':
			key_combination['q'] = 1;
			break;
		case 'w':
			key_combination['w'] = 1;
			break;
		case 'e':
			key_combination['e'] = 1;
			break;
		case 'r':
			key_combination['r'] = 1;
			break;
		case 't':
			key_combination['t'] = 1;
			break;
	}
}

static void on_timer(int value)
{
	/* Proverava se da li callback dolazi od odgovarajuceg tajmera */
	if(value == SUMMON_LINE_TIMER)
	{ 
		/* Postavljaju se pocetne koordinate linije */
		lines[indeks].x_curr = 0.0;
		lines[indeks].y_curr = 2.0 - rect_y/2;
				
		indeks= (++indeks == NUMBER_OF_OBJECTS) ? 0 : indeks;
		
		/* Po potrebi se ponovo postavlja tajmer */
		if(animaton_ongoing)
		{
			glutTimerFunc(TIME_INTERVAL, on_timer, SUMMON_LINE_TIMER);
		}
	}
	if(value == REFRESH_TIMER)
	{	
		angle += 30;
		int k;

		/* Azuriraju se koordinate centra linija */
		for(k=0; k<NUMBER_OF_OBJECTS; k++)
		{
			lines[k].x_curr += v_x;
			lines[k].y_curr += v_y;
		}
		glutPostRedisplay();

		if(animaton_ongoing)
			{
				glutTimerFunc(REFRESH_INTERVAL, on_timer, REFRESH_TIMER);
			}	
	}
	if(value == SUMMON_LINE_OBJECTS_TIMER)
	{		
		/* Postavlja se x koordinata za svaki objekat */
		line_objects[next].x_q = -1.15 + block_x/2;
		line_objects[next].x_w = line_objects[next].x_q + block_x;
		line_objects[next].x_e = line_objects[next].x_q + block_x*2;
		line_objects[next].x_r = line_objects[next].x_q + block_x*3;
		line_objects[next].x_t = line_objects[next].x_q + block_x*4;

		/* Postavlja se y koordinata za celu liniju */
		line_objects[next].y_curr = 2.0 - block_y/2;

		int i, combination = 0;

		/* Pravi se kombinacija objekata na jednoj liniji
		   Oblika: 11001
		   	   qwert
		 */
		for(i=0; i<5; i++)
			combination = (combination | rand()%2) <<1;
		combination = combination>>1;

		/* Generisana kombinacija se dodeljuje trenutnoj liniji sa objektima */
		line_objects[next].combination = combination;
		//printf("%d\n",line_objects[next].combination);

		next = (++next == NUMBER_OF_OBJECTS) ? 0 : next;

		if(animaton_ongoing)
		{
			/* Svaki put se racuna novi interval pravljenja linije sa objektima */
			random_interval = rand()/(float)RAND_MAX * (TIME_INTERVAL - 500) + 300;
			glutTimerFunc(random_interval, on_timer, SUMMON_LINE_OBJECTS_TIMER);
		}
	}
	if(value == OBJECTS_REFRESH_TIMER)
	{
		int k;

		/* Sve linije sa objektima se pomeraju i forsira se iscrtavanje */
		for(k=0; k<NUMBER_OF_OBJECTS; k++)
		{
			line_objects[k].y_curr += v_y;
		}

		glutPostRedisplay();

		if(animaton_ongoing)
		{
			glutTimerFunc(REFRESH_INTERVAL, on_timer, OBJECTS_REFRESH_TIMER);
		}	
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

	draw_valid_destroy_segment();

 	/* Pomocne linije */
	// glLineWidth(2.5);
	// glColor3f(1.0, 0.0, 0.0);
	// glBegin(GL_LINES);
	// glVertex3f(-1.2, -0.5, 0.12);
	// glVertex3f(1.2, -0.5, 0.12);
	// glEnd();

	// glLineWidth(2.5);
	// glColor3f(1.0, 0.0, 0.0);
	// glBegin(GL_LINES);
	// glVertex3f(-1.2, -0.5 + block_x, 0.12);
	// glVertex3f(1.2, -0.5 + block_x, 0.12);
	// glEnd(); 

	/* Crtaju se linije */
	int i;
	for(i=0; i<NUMBER_OF_OBJECTS; i++)
	{
    	draw_line(i);
	}

	/* Crtaju se linije sa objektima */
	for(i=0; i<NUMBER_OF_OBJECTS; i++)
	{
		draw_all_objects(i);
	}

    /* Postavlja se nova slika na prozor */
	glutSwapBuffers();
}
