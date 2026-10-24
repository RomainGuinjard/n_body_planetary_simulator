#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "physics.h"
#include "raylib.h"

float vector_norm(Vector2 vector) {
    /*
    Calculates the euclidean norm of a vector (in 2 dimensions)

    Parameters:
    Vector2 vector : the vector we calculate the norm of

    Outputs:
    float norm : the norm of the vector
    */
    return pow(pow(vector.x, 2) + pow(vector.y, 2), 0.5);
}

Vector2 force_body2_on_body1 (body_t body1, body_t body2) {
    /*
    Calculates the force exerced from the body2 on the body1

    Parameters:
    body_t body1 : the body subject to the force
    body_t body2 : the body exercing the force

    Outputs:
    Vector2 force : 2-dimension vector representing the force
    */
    Vector2 force;
    // we find the direction first
    force.x = body2.position.x - body1.position.x;
    force.y = body2.position.y - body1.position.y;
    // then we determine the norm
    float c = G*body1.mass*body2.mass / pow(vector_norm(force), 3);
    force.x = c*force.x;
    force.y = c*force.y;
    return force;
}

Vector2 force_to_acceleration(Vector2 force, body_t body) {
    /*
    Calculates the acceleration due to the force using Newton's 2nd law of motion.

    Parameters:
    Vector2 force : the force exerced on the body
    body_t body   : the body we want to determine the acceleration

    Outputs:
    Vector2 acceleration : 2-dimension vector representing the acceleration
    */
    Vector2 acceleration;
    acceleration.x = force.x / body.mass;
    acceleration.y = force.y / body.mass;
    return acceleration;
}

void acceleration_to_velocity(Vector2 acceleration, body_t* body, float dt) {
    /*
    Calculates the velocity given acceleration and previous velocity using Euler's method.

    Parameters:
    Vector2 acceleration    : the force acceleration of the body
    body_t body             : the body we want to determine the velocity
    float dt                : the timestep
    */
    body->velocity_tp1.x = body->velocity.x + acceleration.x * dt;
    body->velocity_tp1.y = body->velocity.y + acceleration.y * dt;
}

void velocity_to_position(body_t* body, float dt) {
    /*
    Calculates the position given velocity and previous position using Euler's method.

    Parameters:
    body_t body             : the body we want to determine the velocity
    float dt                : the timestep
    */
    body->position_tp1.x = body->position.x + body->velocity_tp1.x * dt;
    body->position_tp1.y = body->position.y + body->velocity_tp1.y * dt;
}

bool collision_occurred(body_t body1, body_t body2) {
    /*
    Detects whether a collsion occured between two bodies
    */
    float distance = vector_norm((Vector2){body1.position.x - body2.position.x, body1.position.y - body2.position.y});
    return distance <= (body1.radius + body2.radius);
}

body_t collision_new_body(body_t body1, body_t body2) {
    /*
    Creates the new body resulting in the merge of 2 bodies
    */
    body_t new_body;
    strcpy(new_body.label, body1.label);
    strcat(new_body.label, body2.label);
    new_body.mass = body1.mass + body2.mass;
    new_body.radius = pow(pow(body1.radius, 3) + pow(body2.radius, 3), 1.0/3.0);
    new_body.color.r = (body1.color.r * body1.mass + body2.color.r * body2.mass) / new_body.mass;
    new_body.color.g = (body1.color.g * body1.mass + body2.color.g * body2.mass) / new_body.mass;
    new_body.color.b = (body1.color.b * body1.mass + body2.color.b * body2.mass) / new_body.mass;
    new_body.color.a = (body1.color.a * body1.mass + body2.color.a * body2.mass) / new_body.mass;
    new_body.position.x = (body1.position.x*body1.mass + body2.position.x*body2.mass) / new_body.mass;
    new_body.position.y = (body1.position.y*body1.mass + body2.position.y*body2.mass) / new_body.mass;
    new_body.velocity.x = (body1.velocity.x*body1.mass + body2.velocity.x*body2.mass) / new_body.mass;
    new_body.velocity.y = (body1.velocity.y*body1.mass + body2.velocity.y*body2.mass) / new_body.mass;

    new_body.position_tp1 = new_body.position;
    new_body.velocity_tp1 = new_body.velocity;

    new_body.previous_positions = malloc(sizeof(Vector2) * MAX_PREVIOUS);
    for (int k = 0; k < MAX_PREVIOUS; k++) {
        new_body.previous_positions[k] = new_body.position;
    }

    return new_body;
}

void handle_collisions(body_t **tab, int *nbe) {
    for (int i = 0; i < *nbe; i++) {
        for (int j = i + 1; j < *nbe; j++) {
            if (collision_occurred((*tab)[i], (*tab)[j])) {
                body_t new_body = collision_new_body((*tab)[i], (*tab)[j]);

                free((*tab)[i].previous_positions);
                (*tab)[i].previous_positions = NULL;
                free((*tab)[j].previous_positions);
                (*tab)[j].previous_positions = NULL;

                (*tab)[i] = new_body;
                (*tab)[j] = (*tab)[*nbe - 1];
                (*nbe)--;

                body_t *tmp = realloc(*tab, sizeof(body_t) * (*nbe));
                if (tmp != NULL || *nbe == 0) {
                    *tab = tmp;
                }

                i = -1;
                break;
            }
        }
    }
}

Vector2 HeaviestObject(body_t* tab, int* nbe) {
    /*
    Returns the position of the heaviest object contained in tab, which size is nbe
    */
    float maxMass = 0;
    int maxIndex = 0;
    for (int i=0; i< *nbe; i++) {
        if (tab[i].mass > maxMass) {
            maxMass = tab[i].mass;
            maxIndex = i;
        }
    }
    return tab[maxIndex].position;
}

body_t* HandleFarObjects(body_t* tab, int* nbe) {
    /*
    Returns a new tab which is the copy of tab without the furthest body
    */
    body_t* newtab = malloc(sizeof(body_t)*(*nbe));
    Vector2 massCenter;
    massCenter = HeaviestObject(tab, nbe);

    float maxDist = 0.0f;
    float d;
    int maxIndex = -1;
    Vector2 v;

    for (int i=0; i< *nbe; i++) {
        v = (Vector2){tab[i].position.x - massCenter.x, tab[i].position.x - massCenter.y};
        d = vector_norm(v);

        if (d > maxDist) {maxDist = d; maxIndex = i;}
    }

    int c = 0;
    for (int i=0; i< *nbe; i++) {
        if (i != maxIndex) {
            newtab[c] = tab[i];
            c++;
        }
    }
    
    *nbe = *nbe - 1;
    return newtab;
}

void free_bodies(body_t* tab, int nbe) {
    if (tab == NULL) return;

    for (int i = 0; i < nbe; i++) {
        free(tab[i].previous_positions);
        tab[i].previous_positions = NULL;
    }

    free(tab);
}