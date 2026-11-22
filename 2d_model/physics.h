#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"

#ifndef PHYSICS_H
#define PHYSICS_H

#define MAX_PREVIOUS 10000
#define G 6.674e-11

typedef struct body {
    char label[100]; 
    float mass;
    float radius;
    Color color;
    Vector2 position;
    Vector2 velocity;
    Vector2 position_tp1; // Position and velocity at the next step of time
    Vector2 velocity_tp1; 
    Vector2* previous_positions; // used to draw the trails
} body_t;

float vector_norm(Vector2 vector);

Vector2 force_body2_on_body1 (body_t body1, body_t body2);

Vector2 force_to_acceleration(Vector2 force, body_t body);

void acceleration_to_velocity(Vector2 acceleration, body_t* body, float dt);

void velocity_to_position(body_t* body, float dt);

bool collision_occurred(body_t body1, body_t body2);

body_t collision_new_body(body_t body1, body_t body2);

void handle_collisions(body_t **tab, int *nbe);

Vector2 HeaviestObject(body_t* tab, int* nbe);

body_t* HandleFarObjects(body_t* tab, int* nbe);

void free_bodies(body_t* tab, int nbe);

void update_physics(body_t* tab, int nbe, float dt);

#endif