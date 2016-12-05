#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>

#define SUMMON_LINE_TIMER 0
#define REFRESH_TIMER 1
#define SUMMON_LINE_OBJECTS_TIMER 2
#define OBJECTS_REFRESH_TIMER 3
#define TIME_INTERVAL 1000
#define NUMBER_OF_OBJECTS 5

/* Deklaracije callback funkcija */
static void on_keyboard(unsigned char key, int x, int y);
static void keyboard_up(unsigned char key, int x, int y);
static void on_reshape(int width, int hight);
static void on_display(void);
static void on_timer(int value);

/* Deklaracije funkcija */
void draw_line(int line_num);
void draw_blocks(int k);

/* Dimenzije objekta */
const static float rect_x = 2.3;
const static float rect_y = 0.05;

const static float block_x = 2.3/5.0;
const static float block_y = 0.15;

static float v_x, v_y;		  /* Komponente vektora brzine kretanja objekta */
static int animaton_ongoing;  /* Fleg: 1 - animacija je u toku, inace nije */

/* Dimenzije prozora */
static int window_width, window_height;

/* struktura za liniju */
typedef struct 
{
	float x_curr,y_curr;
}line;

/* struktura za liniju sa objektima */
typedef struct 
{
	int keys[256];
	float x_q, x_w, x_e, x_r, x_t, y_curr;
}line_object;

int next = 0;
int index=0;
line_object line_objects[NUMBER_OF_OBJECTS]; 
line lines[NUMBER_OF_OBJECTS];

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

	v_y = -rect_y/2;
	animaton_ongoing=0;

	/* OpenGL inicijalizacija */
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);

	/* Postavljaju se pocetne vrednosti, 
	 	da se ne iscrtava na traci pre pokretanja animacije */
	int k;	
	for(k=0;k<NUMBER_OF_OBJECTS;k++)
	{
		float pom = -3.0;
		lines[k].x_curr = 0.0;
		lines[k].y_curr = pom;

		line_objects[k].x_q = pom;
		line_objects[k].x_w = pom;
		line_objects[k].x_e = pom;
		line_objects[k].x_r = pom;
		line_objects[k].x_t = pom;
	}

	/* Ulazi se u glavnu petlju */
	glutMainLoop();

	return 0;
}

static void keyboard_up(unsigned char key,int x, int y)
{
	line_objects[next].keys['q'] = 0;
	line_objects[next].keys['w'] = 0;
	line_objects[next].keys['e'] = 0;
	line_objects[next].keys['r'] = 0;
	line_objects[next].keys['t'] = 0;

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
				glutTimerFunc(10, on_timer, REFRESH_TIMER);

				glutTimerFunc(2000, on_timer, SUMMON_LINE_OBJECTS_TIMER);
				glutTimerFunc(10, on_timer, OBJECTS_REFRESH_TIMER);
				
				animaton_ongoing = 1;
			}
			break;
		case 'q':
			line_objects[next].keys['q']=1;
			break;
		case 'w':
			line_objects[next].keys['w']=1;
			break;
		case 'e':
			line_objects[next].keys['e']=1;
			break;
		case 'r':
			line_objects[next].keys['r']=1;
			break;
		case 't':
			line_objects[next].keys['t']=1;
			break;

	}
}

static void on_timer(int value)
{
	/* Proverava se da li callback dolazi od odgovarajuceg tajmera */
	if(value == SUMMON_LINE_TIMER)
	{ 
		/* Azuriraju se koordinate centra linije */
		lines[index].x_curr = 0.0;
		lines[index].y_curr = 2.0 - rect_y/2;
				
		index= (++index == NUMBER_OF_OBJECTS) ? 0 : index;
		
		/* Po potrebi se ponovo postavlja tajmer */
		if(animaton_ongoing)
		{
			glutTimerFunc(TIME_INTERVAL, on_timer, SUMMON_LINE_TIMER);
		}
	}
	if(value == REFRESH_TIMER)
	{	
		int k;
		for(k=0;k<NUMBER_OF_OBJECTS;k++)
		{
			lines[k].x_curr += v_x;
			lines[k].y_curr += v_y;
		}
		glutPostRedisplay();

		if(animaton_ongoing)
			{
				glutTimerFunc(10, on_timer, REFRESH_TIMER);
			}	
	}
	if(value == SUMMON_LINE_OBJECTS_TIMER)
	{
		line_objects[next].x_q = -1.15 + block_x/2;
		line_objects[next].x_w = line_objects[next].x_q + block_x;
		line_objects[next].x_e = line_objects[next].x_q + block_x*2;
		line_objects[next].x_r = line_objects[next].x_q + block_x*3;
		line_objects[next].x_t = line_objects[next].x_q + block_x*4;
		line_objects[next].y_curr = 2.0 - block_y/2;

		next = (++next == NUMBER_OF_OBJECTS) ? 0 : next;

		if(animaton_ongoing)
		{
			int r = rand()/(float)RAND_MAX * (TIME_INTERVAL - 500) + 500;
			//printf("rand:%d\n", r);
			glutTimerFunc(r, on_timer, SUMMON_LINE_OBJECTS_TIMER);
		}
	}
	if(value == OBJECTS_REFRESH_TIMER)
	{
		int k;
		for(k=0;k<NUMBER_OF_OBJECTS;k++)
		{
			line_objects[k].y_curr += v_y;
		}
		glutPostRedisplay();

		if(animaton_ongoing)
		{
			glutTimerFunc(10, on_timer, OBJECTS_REFRESH_TIMER);
		}	
	}
}

static void on_reshape(int width, int height)
{
	window_width = width;
	window_height = height;
}

/* Iscrtava se linija */
void draw_line(int line_num)
{
	glColor3f(0.2,0.6,1);
    glBegin(GL_POLYGON);
        glVertex3f(lines[line_num].x_curr - rect_x/2,
        			 lines[line_num].y_curr - rect_y/2, 0.11);
        glVertex3f(lines[line_num].x_curr + rect_x/2,
        			 lines[line_num].y_curr - rect_y/2, 0.11);
        glVertex3f(lines[line_num].x_curr + rect_x/2, 
        			 lines[line_num].y_curr + rect_y/2, 0.11);
        glVertex3f(lines[line_num].x_curr - rect_x/2,
        			 lines[line_num].y_curr + rect_y/2, 0.11);
    glEnd();

	glutPostRedisplay();
}

/* Iscrtava se linija sa objektima */
void draw_blocks(int k)
{
	//prvi blok : q
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
        glVertex3f(line_objects[k].x_q - block_x/2,
        			 line_objects[k].y_curr - block_y/2, 0.12);
        glVertex3f(line_objects[k].x_q + block_x/2,
         			 line_objects[k].y_curr - block_y/2, 0.12);
        glVertex3f(line_objects[k].x_q + block_x/2, 
        			 line_objects[k].y_curr + block_y/2, 0.12);
        glVertex3f(line_objects[k].x_q - block_x/2,
        			 line_objects[k].y_curr + block_y/2, 0.12);
    glEnd();

    //drugi blok : w
    glColor3f(1.0,1.0,0.0);
	glBegin(GL_POLYGON);
        glVertex3f(line_objects[k].x_w - block_x/2,
        			 line_objects[k].y_curr - block_y/2, 0.12);
        glVertex3f(line_objects[k].x_w + block_x/2,
         			 line_objects[k].y_curr  - block_y/2, 0.12);
        glVertex3f(line_objects[k].x_w + block_x/2, 
        			 line_objects[k].y_curr + block_y/2, 0.12);
        glVertex3f(line_objects[k].x_w - block_x/2,
        			 line_objects[k].y_curr + block_y/2, 0.12);
    glEnd();

    //treci blok : e
    glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_POLYGON);
        glVertex3f(line_objects[k].x_e - block_x/2,
        			 line_objects[k].y_curr - block_y/2, 0.12);
        glVertex3f(line_objects[k].x_e + block_x/2,
         			 line_objects[k].y_curr - block_y/2 , 0.12);
        glVertex3f(line_objects[k].x_e + block_x/2, 
        			 line_objects[k].y_curr + block_y/2, 0.12);
        glVertex3f(line_objects[k].x_e - block_x/2,
        			 line_objects[k].y_curr + block_y/2, 0.12);
    glEnd();

    //cetvrti blok : r
    glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_POLYGON);
        glVertex3f(line_objects[k].x_r - block_x/2,
        			 line_objects[k].y_curr - block_y/2, 0.12);
        glVertex3f(line_objects[k].x_r + block_x/2,
         			 line_objects[k].y_curr - block_y/2 , 0.12);
        glVertex3f(line_objects[k].x_r + block_x/2, 
        			 line_objects[k].y_curr + block_y/2, 0.12);
        glVertex3f(line_objects[k].x_r - block_x/2,
        			 line_objects[k].y_curr + block_y/2, 0.12);
    glEnd();

    //peti blok : t
    glColor3f(1.0, 0.5, 0.0);
	glBegin(GL_POLYGON);
        glVertex3f(line_objects[k].x_t - block_x/2,
        			 line_objects[k].y_curr - block_y/2, 0.12);
        glVertex3f(line_objects[k].x_t + block_x/2,
         			 line_objects[k].y_curr - block_y/2 , 0.12);
        glVertex3f(line_objects[k].x_t + block_x/2, 
        			 line_objects[k].y_curr + block_y/2, 0.12);
        glVertex3f(line_objects[k].x_t - block_x/2,
        			 line_objects[k].y_curr + block_y/2, 0.12);
    glEnd();

	glutPostRedisplay();
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

	/* Crtaju se linije */
	int i;
	for(i=0;i<NUMBER_OF_OBJECTS;i++)
	{
    	draw_line(i);
	}

	/* Crtaju se linije sa objektima */
	for(i=0;i<NUMBER_OF_OBJECTS;i++)
	{
		draw_blocks(i);
		// printf("q: %d w: %d e: %d r: %d t: %d \n",line_objects[i].keys['q'],
		// 	line_objects[i].keys['w'], line_objects[i].keys['e'], line_objects[i].keys['r'],
		// 	line_objects[i].keys['t']);
	}

    /* Postavlja se nova slika na prozor */
	glutSwapBuffers();
}