#ifndef DATA3D_H
#define DATA3D_H

#include <stdio.h>
#include "raylib.h"
#include "physics3d.h"

typedef enum ScaleType {
    SCALE_LINEAR,
    SCALE_LOG,
    SCALE_SQRT
} ScaleType;

body_t* collecting_data(FILE* f, int* nbe);

void virtual3d(float* radius,
               Vector3* position,
               ScaleType scaleDist,
               ScaleType scaleRad,
               float radius_scale,
               float dist_scale,
               float original_radius,
               Vector3 original_position,
               float AU);

float distancemax(body_t* tab, int nbe);

#endif
