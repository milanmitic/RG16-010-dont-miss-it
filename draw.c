#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <time.h>
#include <math.h>

#include "draw.h"

/* Dimenzije linije */
const float rect_x = 2.3;
const float rect_y = 0.05;

/* Dimenzije jednog bloka */
const float block_x = 2.3/5.0;
const float block_y = 0.15;

int animaton_ongoing;  /* Fleg: 1 - animacija je u toku, inace nije */

line_object line_objects[NUMBER_OF_OBJECTS]; 
line lines[NUMBER_OF_OBJECTS];
int random_interval;
float angle = 0.0;
int next = 0;
int indeks = 0;

int current_score = 0; 
char text[50];
int new_time;
int old_time;

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

/* Prvi argument sluzi radi odredjivanja boje koja se koristi unutar f-je.
   Treci i cetvrti argument odredjuju za koliko se svaki deo objekta translira
*/
void draw_object(char character, GLUquadric* obj, float current_obj_x_axis,
				float currrent_obj_y_axis)
{
	float radius1 = block_x/2 - 0.02;
	float radius2 = radius1 - 0.09;

	/* Crta se prvi deo objekta - poklopac */
	glPushMatrix();
		/* Postavlja se odgovarajuca boja u zavisnosti od trake */
		if(character == 'q')
			glColor3f(1.0, 0.0, 0.0);
		else if(character == 'w')
		 	glColor3f(1.0, 1.0, 0.0);
		else if(character == 'e')
		 	glColor3f(0.0, 0.0, 1.0);
		else if(character == 'r')
		 	glColor3f(0.0, 1.0, 0.0);
		else if(character == 't')
	    	glColor3f(1.0, 0.5, 0.0);
	    else if(character == 'd')
	    	glColor3f(0, 0, 0);

	    /* Transliramo poklopac kako bi se nalazio u odgovarajucoj traci */
        glTranslatef(current_obj_x_axis, currrent_obj_y_axis, 0.32);
        /* Ugao rotacije se racuna kao 30 * angle/360 => angle/12 */
        glRotatef(angle/12, 0, 0, 1);
        /* Crta se poklopac */
        gluCylinder(obj, radius2,  0, 0, 9, 10);
    glPopMatrix();

    /* Crta se drugi deo objekta - valjak */
	glPushMatrix();
		if(character == 'q')
			glColor3f(1.0, 0.0, 0.0);
		else if(character == 'w')
		 	glColor3f(1.0, 1.0, 0.0);
		else if(character == 'e')
		 	glColor3f(0.0, 0.0, 1.0);
		else if(character == 'r')
		 	glColor3f(0.0, 1.0, 0.0);
		else if(character == 't')
	    	glColor3f(1.0, 0.5, 0.0);
	    else if(character == 'd')
	    	glColor3f(0, 0, 0);

        glTranslatef(current_obj_x_axis, currrent_obj_y_axis, 0.28);
		glRotatef(angle/12, 0, 0, 1);
        gluCylinder(obj, radius2,  radius2, 0.04, 9, 10);
    glPopMatrix();

    /* Crta se trecu deo objekta - valjak sa razlicitim poluprecnicima osnovica */
    glPushMatrix();
		glColor3f(0.5, 0.5, 0.5);
		if(character == 'd')
	    	glColor3f(0, 0, 0);

        glTranslatef(current_obj_x_axis, currrent_obj_y_axis, 0.21);
        glRotatef(angle/12, 0, 0, 1);
        gluCylinder(obj, radius1, radius2, 0.07, 9, 10);
    glPopMatrix();

    /* Crta se cevrti deo objekta - valjak */
    glPushMatrix();
        if(character == 'q')
			glColor3f(1.0, 0.0, 0.0);
		else if(character == 'w')
		 	glColor3f(1.0, 1.0, 0.0);
		else if(character == 'e')
		 	glColor3f(0.0, 0.0, 1.0);
		else if(character == 'r')
		 	glColor3f(0.0, 1.0, 0.0);
		else if(character == 't')
	    	glColor3f(1.0, 0.5, 0.0);
	    else if(character == 'd')
	    	glColor3f(0, 0, 0);

        glTranslatef(current_obj_x_axis, currrent_obj_y_axis, 0.11);
        glRotatef(angle/12, 0, 0, 1);
        gluCylinder(obj, radius1,  radius1, 0.10, 9, 10);
    glPopMatrix();
}

	
/* Crta se jedini validni segment za unistavanje objekata.
   U segmentu ima po jedno polje za svaku traku */
void draw_valid_destroy_segment(void)
{
	GLUquadric* obj2 = gluNewQuadric();
	float x_coord;
	float x_coord_q = -1.15 + block_x/2;
	float radius = block_x/2 - 0.02;

	/* Crta se polje za svaku traku */
	int i;
	for(i=0; i<NUMBER_OF_OBJECTS; i++)
    {
    	/* Polje se postavlja na odgovarajucu boju i poziciju, a onda crta */
	    glPushMatrix();
	        if(i == 0)
	        {
		        x_coord = x_coord_q;
				glColor3f(1.0, 0.0, 0.0);
	        }
			else if(i == 1)
			{
				x_coord = x_coord_q + block_x;
				glColor3f(1.0, 1.0, 0.0);
			}
			else if(i == 2)
			{
				x_coord = x_coord_q + block_x*2;
				glColor3f(0.0, 0.0, 1.0);
			}
			else if(i == 3)
			{
				x_coord = x_coord_q + block_x*3;
				glColor3f(0.0, 1.0, 0.0);
			}
			else if(i == 4)
			{
				x_coord = x_coord_q + block_x*4;
		    	glColor3f(1.0, 0.5, 0.0);
			}

	         glTranslatef(x_coord, -0.27, 0.12);
	         glRotatef(angle/12, 0, 0, 1);
	         gluCylinder(obj2, radius, radius - 0.06, 0, 9, 10);
	    glPopMatrix();

	    /* Crta se crni deo polja */
	    glPushMatrix();
		     glColor3f(0, 0, 0);
	         glTranslatef(x_coord, -0.27, 0.12);
	         glRotatef(angle/12, 0, 0, 1);
	         gluCylinder(obj2, radius,  0, 0, 9, 10);
    	glPopMatrix();
	}
}


/* Iscrtava se linija sa objektima. F-ja prima broj linije
   koju treba da iscrta i indikator (1 - linija unistena inace 0) */
void draw_all_objects(int k, int indicator)
{
	GLUquadric* obj = gluNewQuadric();

	/* Za trenutnu liniju se proverava koji su aktivni i ti se iscrtavaju.
	   Vrsi se provera po bitovima: 1 - objekat postoji, 0 - ne postoji
	   Kombinacija je oblika: 11001
	  						  qwert 
	*/
    /* Ako je indikator 1 f-ji "draw_object" se salje kao prvi argument 
       karakter 'd' ("destroyed"), a inace odgovarajuci karakter za tu kolonu
    */

	/* Prvi objekat: q */
	if(line_objects[k].combination & (1<<4))
	{
		draw_object((indicator == 1) ? 'd' : 'q', obj, line_objects[k].x_q, line_objects[k].y_curr);
 	}

    /* Drugi objekat: w */
    if(line_objects[k].combination & (1<<3))
    {
		draw_object((indicator == 1) ? 'd' : 'w', obj, line_objects[k].x_w, line_objects[k].y_curr);
    }

    /* Treci objekat: e */
    if(line_objects[k].combination & (1<<2))
	{
		draw_object((indicator == 1) ? 'd' : 'e', obj, line_objects[k].x_e, line_objects[k].y_curr);
	}

    /* Cetvrti objekat: r */
    if(line_objects[k].combination & (1<<1))
	{
		draw_object((indicator == 1) ? 'd' : 'r', obj, line_objects[k].x_r, line_objects[k].y_curr);
	}

    /* Peti objekat: t */
    if(line_objects[k].combination & 1)
	{
		draw_object((indicator == 1) ? 'd' : 't', obj, line_objects[k].x_t, line_objects[k].y_curr);
	}

	glutPostRedisplay();
}


/* F-ja za ispis rezultata */
void draw_score()
{
	int sec_remaining;
	glPushMatrix();
		glTranslatef(-0.5, 1, 0);
		glColor3f(1, 1, 1);
		glRotatef(30, 0, 0, -1);
		glRasterPos3f(-1.2, 0, 1);
	glPopMatrix();

	/* Svaki put se oduzima broj ms proteklih od 
		pocetka programa do pokretanja igre */
	if(animaton_ongoing)
		new_time = glutGet(GLUT_ELAPSED_TIME) - old_time;

	/* Ispis trenutnog rezultata i broj sekundi do kraja */
	sec_remaining = (60000 - new_time)/1000;
	sprintf(text, "Score: %d  Seconds remaining: %d", current_score, sec_remaining);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, text);

	/* Igra traje 1 min */
	if(sec_remaining == 0)
	{
		printf("\n\t\t\tVas rezultat je: %d\n\n", current_score);
		exit(0);
	}
	
	glutPostRedisplay();
}