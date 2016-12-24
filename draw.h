#ifndef DRAW_H
	#define DRAW_H

	#define NUMBER_OF_OBJECTS 5

	void draw_line(int line_num);
	void draw_object(char character, GLUquadric* obj, float current_obj_x_axis,
				float currrent_obj_y_axis);
	void draw_valid_destroy_segment(void);
	void draw_all_objects(int k, int indicator);
	void draw_score(void);

	/* Struktura za liniju */
	typedef struct 
	{
		float x_curr,y_curr;
	}line;

	/* Struktura za liniju sa objektima */
	typedef struct 
	{
		float x_q, x_w, x_e, x_r, x_t, y_curr;
		int combination;
		int destroyed;
	}line_object;


#endif