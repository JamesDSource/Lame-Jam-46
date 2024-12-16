#include<stdio.h>
#include<math.h>
#include "collision.h"

static float min_max_intersection(float min0, float max0, float min1, float max1) {
	if((min1 < max0 && min1 > min0) || (min0 < max1 && min0 > min1)) {
		float mag0 = max0 - min1;
		float mag1 = min0 - max1;
		return (fabs(mag0) < fabs(mag1)) ? mag0 : mag1;
	}
	return 0.0f;
}

bool box_box_collision(Rectangle b0, Rectangle b1, Vector2 *push_out) {
	float min_x0, max_x0, min_y0, max_y0;
	float min_x1, max_x1, min_y1, max_y1;
	min_x0 = b0.x;
	max_x0 = b0.x + b0.width;
	min_y0 = b0.y;
	max_y0 = b0.y + b0.height;
	min_x1 = b1.x;
	max_x1 = b1.x + b1.width;
	min_y1 = b1.y;
	max_y1 = b1.y + b1.height;
	float intersec_x = min_max_intersection(min_x0, max_x0, min_x1, max_x1);
	float intersec_y = min_max_intersection(min_y0, max_y0, min_y1, max_y1);
	if(fabs(intersec_x) < 0.00001f && fabs(intersec_y) < 0.00001f) {
		return false;
	} else if(fabs(intersec_x) < fabs(intersec_y)){
		if(push_out != NULL) {
			push_out->x = intersec_x;
			push_out->y = 0.0f;
		}
		return intersec_x != 0.0f;
	} else {
		if(push_out != NULL) {
			push_out->x = 0.0f;
			push_out->y = intersec_y;
		}
		return intersec_y != 0.0f;
	}
}
