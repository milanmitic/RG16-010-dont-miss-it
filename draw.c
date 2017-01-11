#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <time.h>
#include <math.h>

#include "draw.h"

#define PI 3.1415926535

/* Dimenzije linije */
const float rect_x = 2.3;
const float rect_y = 0.05;

/* Dimenzije jednog bloka */
const float block_x = 2.3/5.0;
const float block_y = 0.15;

/* Fleg: 1 - animacija je u toku, inace nije */
int animaton_ongoing;

line_object line_objects[NUMBER_OF_OBJECTS]; 
line lines[NUMBER_OF_OBJECTS];

int random_interval;
float angle = 0.0;
int next = 0;
int indeks = 0;

/* Promenljive za rezultat i preostalo vreme */
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

void draw_cylinder(double top_radius, double base_radius, double height, double slices)
{
	int i;
	float vecA[3], vecB[3], vecN[3];

	glRotatef(90,1,0,0);

	glBegin(GL_TRIANGLE_STRIP);
		/* Za jedan vise kako bi se iscrtao zatvoren cilindar */
		for (i=0; i<=slices; i++) 
		{
			float x_coord1, z_coord1;
			float x_coord2, z_coord2;

			x_coord1 = cos(2 * i * PI / slices) * top_radius;
			z_coord1 = sin(2 * i * PI / slices) * top_radius;

			x_coord2 = cos(2 * i * PI / slices) * base_radius;
			z_coord2 = sin(2 * i * PI / slices) * base_radius;

			/* Racunanje vektora normale koji je potreban zbog osvetljenja */

			/* Prvi vektor */
			vecA[0] = x_coord2 - x_coord1;
			vecA[1] = height;
			vecA[2] = z_coord2 - z_coord1; 

			/* Drugi vektor */
			vecB[0] = cos(2 * (i + 1) * PI / slices) * base_radius - x_coord1;
			vecB[1] = height;
			vecB[2] = sin(2 * (i + 1) * PI / slices) * base_radius - z_coord1; 

			/* Vektorski proizvod */
			vecN[0] = vecA[1] * vecB[2] - vecB[1] * vecA[2];
			vecN[1] = vecA[0] * vecB[2] - vecB[0] * vecA[2];
			vecN[2] = vecA[0] * vecB[1] - vecB[0] * vecA[1];

			/* Normalizacija vektora */
			float length = 
				sqrt(vecN[0] * vecN[0] + vecN[1] * vecN[1] + vecN[2] * vecN[2]);

			glNormal3f(vecN[0] / length, vecN[1] / length, vecN[2] / length);

			/* Zadajemo tacke za crtanje cilindra */
			glVertex3f(
					x_coord1,
					height, 
					z_coord1);
			glVertex3f(
					x_coord2,
					0,
					z_coord2);

		}

    glEnd();

}


/* Prvi argument sluzi radi odredjivanja boje koja se koristi unutar f-je.
   Drugi i treci argument odredjuju za koliko se svaki deo objekta translira
*/
void draw_object(char character, float current_obj_x_axis,
				float currrent_obj_y_axis)
{
	float radius1 = block_x/2 - 0.02;
	float radius2 = radius1 - 0.09;

	/* Koeficijenti ambijentalne refleksije materijala */
	GLfloat ambient_coeffs[] = { 0.1, 0.1, 0.1, 1 };

	/* Koeficijenti difuzne refleksije materijala */
	GLfloat diffuse_coeffs[] = { 0.0, 0.0, 0.8, 1 };

	/* Koeficijenti spekularne refleksije materijala */
	GLfloat specular_coeffs[] = { 1, 1, 1, 1 };

	/* Koeficijent glatkosti materijala */
	GLfloat shininess = 20;

	/* Podesavaju se parametri materijala */
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	glEnable(GL_COLOR_MATERIAL);

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
		draw_cylinder(0, radius2, 0, 8);
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
		draw_cylinder(radius2, radius2, 0.04, 8);
	glPopMatrix();

	/* Crta se treci deo objekta - valjak sa razlicitim poluprecnicima osnovica */
	glPushMatrix();
		glColor3f(0.5, 0.5, 0.5);
		if(character == 'd')
			glColor3f(0, 0, 0);

        glTranslatef(current_obj_x_axis, currrent_obj_y_axis, 0.21);
        glRotatef(angle/12, 0, 0, 1);
    	draw_cylinder(radius2, radius1, 0.07, 8);
    glPopMatrix();

	/* Crta se cetvrti deo objekta - valjak */
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
	draw_cylinder(radius1, radius1, 0.1, 8);
	glPopMatrix();

	glDisable(GL_COLOR_MATERIAL);
}
	
/* Crta se jedini validni segment za unistavanje objekata.
   U segmentu ima po jedno polje za svaku traku */
void draw_valid_destroy_segment(void)
{
	float x_coord;
	float x_coord_q = -1.15 + block_x/2;
	float radius = block_x/2 - 0.02;

	glEnable(GL_COLOR_MATERIAL);

	int i;
	/* Crta se polje za svaku traku */
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
			draw_cylinder(radius - 0.06, radius, 0 ,9);
		glPopMatrix();

		/* Crta se crni deo polja */
		glPushMatrix();
			glColor3f(0, 0, 0);
			glTranslatef(x_coord, -0.27, 0.12);
			glRotatef(angle/12, 0, 0, 1);
			draw_cylinder(0, radius, 0, 9);
		glPopMatrix();
	}

	glDisable(GL_COLOR_MATERIAL);
	glutPostRedisplay();
}


/* Iscrtava se linija sa objektima. F-ja prima broj linije
   koju treba da iscrta i indikator (1 - linija unistena inace 0) */
void draw_all_objects(int k, int indicator)
{
	/* Za trenutnu liniju se proverava koji su aktivni i ti se iscrtavaju.
	   Vrsi se provera po bitovima: 1 - objekat postoji, 0 - ne postoji
	   Kombinacija je oblika: 11001
	  						  qwert 
	*/

	/* Ako je indikator 1, f-ji "draw_object" se salje kao prvi argument 
	   karakter 'd' ("destroyed"), a inace odgovarajuci karakter za tu kolonu
	*/

	/* Prvi objekat: q */
	if(line_objects[k].combination & (1<<4))
	{
		draw_object((indicator == 1) ? 'd' : 'q',
			line_objects[k].x_q, line_objects[k].y_curr);
	}

	/* Drugi objekat: w */
	if(line_objects[k].combination & (1<<3))
	{
		draw_object((indicator == 1) ? 'd' : 'w',
			line_objects[k].x_w, line_objects[k].y_curr);
	}

	/* Treci objekat: e */
	if(line_objects[k].combination & (1<<2))
	{
		draw_object((indicator == 1) ? 'd' : 'e',
			line_objects[k].x_e, line_objects[k].y_curr);
	}

	/* Cetvrti objekat: r */
	if(line_objects[k].combination & (1<<1))
	{
		draw_object((indicator == 1) ? 'd' : 'r',
			line_objects[k].x_r, line_objects[k].y_curr);
	}

	/* Peti objekat: t */
	if(line_objects[k].combination & 1)
	{
		draw_object((indicator == 1) ? 'd' : 't',
			line_objects[k].x_t, line_objects[k].y_curr);
	}

	glutPostRedisplay();
}

/* F-ja za ispis rezultata */
void draw_score()
{
	int sec_remaining;

	glEnable(GL_COLOR_MATERIAL);
	
	/* Postavlja se tekst na odgovarajucu poziciju */
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

	glDisable(GL_COLOR_MATERIAL);	
	glutPostRedisplay();
}
