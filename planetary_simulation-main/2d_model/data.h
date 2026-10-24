#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "physics.h"

#ifndef DATA_H
#define DATA_H

#define MAX_PREVIOUS 10000

body_t* collecting_data(FILE *f,int* nbe);

typedef enum ScaleType {
    SCALE_LINEAR,
    SCALE_LOG,
    SCALE_SQRT
} ScaleType;

void virtual(float* radius, Vector2* position, ScaleType scaleDist, ScaleType scaleRad, float radius_scale, float dist_scale, float original_radius, Vector2 original_position, float AU);

float distancemax(body_t* tab, int nbe);

#endif