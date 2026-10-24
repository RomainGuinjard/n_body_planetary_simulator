#ifndef PHYSICS3D_H
#define PHYSICS3D_H

#include <stdbool.h>
#include "raylib.h"

#define MAX_PREVIOUS 10000
#define G 6.674e-11

typedef struct body {
    char label[100];
    float mass;
    float radius;
    Color color;

    Vector3 position;
    Vector3 velocity;

    Vector3 position_tp1;
    Vector3 velocity_tp1;

    Vector3* previous_positions;
} body_t;

float vector_norm3d(Vector3 vector);

Vector3 force_body2_on_body1(body_t body1, body_t body2);

Vector3 force_to_acceleration(Vector3 force, body_t body);

void acceleration_to_velocity(Vector3 acceleration, body_t* body, float dt);

void velocity_to_position(body_t* body, float dt);

bool collision_occurred(body_t body1, body_t body2);

body_t collision_new_body(body_t body1, body_t body2);

void handle_collisions(body_t** tab, int* nbe);

void free_bodies(body_t* tab, int nbe);

void update_physics(body_t* tab, int nbe, float dt);

#endif
