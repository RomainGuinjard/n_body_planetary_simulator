#ifndef CAMERA3D_UTILS_H
#define CAMERA3D_UTILS_H

#include "raylib.h"
#include "physics3d.h"
#include "data3d.h"

void reset_camera(Camera3D* camera);

float dot_product_3d(Vector3 a, Vector3 b);

int select_body_3d(Vector2 mouse,
                   Camera3D camera,
                   body_t* tab,
                   int nbe,
                   ScaleType scaledist,
                   ScaleType scalerad,
                   float radius_scale,
                   float dist_scale,
                   float AU);

void follow_body_3d(Camera3D* camera,
                    body_t body,
                    ScaleType scaledist,
                    ScaleType scalerad,
                    float radius_scale,
                    float dist_scale,
                    float AU);

#endif