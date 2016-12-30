#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <time.h>
#include <math.h>

#include "draw.h"

#define SUMMON_LINE_TIMER 0
#define REFRESH_TIMER 1
#define SUMMON_LINE_OBJECTS_TIMER 2
#define OBJECTS_REFRESH_TIMER 3

#define TIME_INTERVAL 1000
#define REFRESH_INTERVAL 1

/* Deklaracije callback funkcija */
static void on_keyboard(unsigned char key, int x, int y);
static void keyboard_up(unsigned char key, int x, int y);
static void on_reshape(int width, int hight);
static void on_display(void);
static void on_timer(int value);
static int generate_combination(void);

/* Komponente vektora brzine kretanja objekta */
static float v_x, v_y;

/* Dimenzije prozora */
static int window_width, window_height;

/* Promenljiva za pamcenje kombinacije tastera */
int key_combination;

/* Ugao rotacije oko y-ose */
double rotate_angle = 15;

/* od 0.2 do 1.0 */
double rotate_coeff = 0.2;

/* Promenljiva koja ima vrednost 10, 20, 30... */
int score_checkpoint = 10;

extern float rect_y;
extern float block_x;
extern float block_y;
extern line lines[];
extern line_object line_objects[];
extern int next;
extern float angle;
extern int random_interval;
extern int indeks;
extern int animaton_ongoing;
extern int old_time;
extern int current_score;

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
	old_time = glutGet(GLUT_ELAPSED_TIME);

	/* OpenGL inicijalizacija */
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);

	/* Ulazi se u glavnu petlju */
	glutMainLoop();

	return 0;
}

static void keyboard_up(unsigned char key,int x, int y)
{
	/* Resetuju se pritisnuti tasteri */

	/* Dovoljno je samo celu kombinaciju postaviti na 0 
	   posto je kombinacija bitova
	*/
	key_combination = 0;
}

static void on_keyboard(unsigned char key, int x, int y)
{
	/* U zavisnosti od pritisnutog tastera izvrsava se odgovarajuca akcija */
	if(key == 27)
	/* Zavrsava se program */
	{
		exit(0);
	}	

	if(key == 'g')
	{
		/* Pokrece se animacija */
		if(!animaton_ongoing)
		{
			glutTimerFunc(TIME_INTERVAL, on_timer, SUMMON_LINE_TIMER);
			glutTimerFunc(REFRESH_INTERVAL, on_timer, REFRESH_TIMER);

			glutTimerFunc(2000, on_timer, SUMMON_LINE_OBJECTS_TIMER);
			glutTimerFunc(REFRESH_INTERVAL, on_timer, OBJECTS_REFRESH_TIMER);

			/* Pamtimo broj ms proteklih od pokretanja programa */
			old_time = glutGet(GLUT_ELAPSED_TIME);

			animaton_ongoing = 1;
		}
	}

	int i;
	for(i=0; i<NUMBER_OF_OBJECTS; i++)		
	{
		/* U zavisnosti od pritisnutih tastera se pravi kombinacija */
		if(line_objects[i].y_curr <= -0.5 + block_x &&
			line_objects[i].y_curr >= -0.5)
		{
			if(key == 'q')
				key_combination |= (1<<4);				
			if(key == 'w')
				key_combination |= (1<<3);
			if(key == 'e')
				key_combination |= (1<<2);
			if(key == 'r')
				key_combination |= (1<<1);
			if(key == 't')
				key_combination |= 1;

			/* Pogodjena je odgovarajuca kombinacija */
			if(key_combination != 0 && 
				(key_combination == line_objects[i].combination))
			{
				/* Uvecava se ukupan rezultat i postavlja indikator
				   da je ta linija unistena
				 */
				current_score += 1;
				line_objects[i].destroyed = 1;
					
				/* Brzina rotacije se poveca kada rezultat bude 10, 20, 30... */
				if(rotate_coeff<1.0 && current_score == score_checkpoint)
				{
					if(rotate_coeff>0)
					{
						rotate_coeff += 0.1;		
						score_checkpoint += 10;	
					}
					else if(rotate_coeff <= 0)
					{
						rotate_coeff -= 0.1;
						score_checkpoint += 10;
					}
				}
			}
		}
	}
}

/* Pravi se kombinacija aktivnih objekata na jednoj liniji
   Oblika: 11001
   		   qwert
 */
static int generate_combination()
{
	int counter_1 = 0;
	int pom = 0;
	int i, combination = 0;

	for(i=0; i<NUMBER_OF_OBJECTS; i++)
	{
		/* 0 ili 1 se stavlja na jedno od 5 mesta */
		pom = rand()%2 << rand()%5;

		/* Prekida se generisanje ako vec ima 3 jedinice */
		if(pom != 0)
			counter_1++;
		if(counter_1 == 4)
			break;

		/* Dodaje se na konacnu kombinaciju bitova */
		combination = (combination | pom);
	}

	return combination;
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

		/* Postavlja se indikator da li je objekat unesten na 0 */
		line_objects[next].destroyed = 0;


		/* Generise se kombinacija i dodeljuje trenutnoj liniji sa objektima */
		line_objects[next].combination = generate_combination();
		
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
	/* Pozicija svetla */
	GLfloat light_position[] = { 1, 1, 1, 0 };

    /* Ambijentalna boja svetla */
	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1 };

    /* Difuzna boja svetla */
	GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };

    /* Spekularna boja svetla */
	GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

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

	/* Ukljucuje se osvetljenje i podesavaju parametri svetla */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	
	/* Crta se rezultat */
	draw_score();

	/* Rotira se sve na sceni */
	if((int)rotate_angle == 20 || (int)rotate_angle == -20)
	{
		rotate_coeff *= -1;
	}

	rotate_angle = rotate_angle + rotate_coeff;
	glRotatef(rotate_angle, 0, 1, 0);

	/* Crtaju se validne pozicije za unistavanje objekata */
	draw_valid_destroy_segment();

	/* Crtaju se linije */
	int i;
	for(i=0; i<NUMBER_OF_OBJECTS; i++)
	{
		draw_line(i);
	}

	/* Crtaju se linije sa objektima */
	for(i=0; i<NUMBER_OF_OBJECTS; i++)
	{
		if(line_objects[i].destroyed == 1)
		{
			draw_all_objects(i, 1);
		}
		else
		{
			draw_all_objects(i, 0);	
		}
	}

	/* Crta se osnovna traka */
	glBegin(GL_QUADS);
	 	glColor3f(0.3, 1.0, 1.0);     
	 	glVertex3f(-1.2, -1.0, 0);
	 	glVertex3f(-1.2, 4.0, 0);
	 	glVertex3f(1.2, 4.0, 0);
	 	glVertex3f(1.2, -1.0, 0);	
	glEnd();

	/* Postavlja se nova slika na prozor */
	glutSwapBuffers();
}


