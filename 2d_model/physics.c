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

Vector2 force_body2_on_body1(body_t body1, body_t body2) {

    double dx = (double)body2.position.x - (double)body1.position.x;
    double dy = (double)body2.position.y - (double)body1.position.y;

    double distance = sqrt(dx * dx + dy * dy);

    if (distance <= 0.0) {
        return (Vector2){0.0f, 0.0f};
    }

    double coefficient = G * (double)body1.mass * (double)body2.mass
                       / (distance * distance * distance);

    Vector2 force;
    force.x = (float)(coefficient * dx);
    force.y = (float)(coefficient * dy);

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

    if (body.mass <= 0.0f) {
        return (Vector2){0.0f, 0.0f};
    }

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

    snprintf(new_body.label, sizeof(new_body.label),
             "%.40s_%.40s", body1.label, body2.label);

    double mass1 = (double)body1.mass;
    double mass2 = (double)body2.mass;
    double total_mass = mass1 + mass2;

    if (total_mass <= 0.0) {
        fprintf(stderr, "Invalid mass in collision_new_body\n");
        exit(EXIT_FAILURE);
    }

    new_body.mass = (float)total_mass;

    new_body.radius = powf(powf(body1.radius, 3.0f)
                         + powf(body2.radius, 3.0f),
                           1.0f / 3.0f);

    new_body.color.r = (unsigned char)(
        (body1.color.r * mass1 + body2.color.r * mass2) / total_mass);

    new_body.color.g = (unsigned char)(
        (body1.color.g * mass1 + body2.color.g * mass2) / total_mass);

    new_body.color.b = (unsigned char)(
        (body1.color.b * mass1 + body2.color.b * mass2) / total_mass);

    new_body.color.a = 255;

    new_body.position.x = (float)(
        (body1.position.x * mass1 + body2.position.x * mass2) / total_mass);

    new_body.position.y = (float)(
        (body1.position.y * mass1 + body2.position.y * mass2) / total_mass);

    new_body.velocity.x = (float)(
        (body1.velocity.x * mass1 + body2.velocity.x * mass2) / total_mass);

    new_body.velocity.y = (float)(
        (body1.velocity.y * mass1 + body2.velocity.y * mass2) / total_mass);

    new_body.position_tp1 = new_body.position;
    new_body.velocity_tp1 = new_body.velocity;

    new_body.previous_positions = malloc(sizeof(Vector2) * MAX_PREVIOUS);

    if (new_body.previous_positions == NULL) {
        fprintf(stderr, "Allocation error in collision_new_body\n");
        exit(EXIT_FAILURE);
    }

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
        v = (Vector2){tab[i].position.x - massCenter.x, tab[i].position.y - massCenter.y};
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
    
    if (maxIndex >= 0) {
        free(tab[maxIndex].previous_positions);
        *nbe = *nbe - 1;
    }
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

void update_physics(body_t* tab, int nbe, float dt) {

    // Calculate the next state of each body
    for (int i = 0; i < nbe; i++) {
        Vector2 total_force = {0.0f, 0.0f};

        for (int j = 0; j < nbe; j++) {
            if (i != j) {
                Vector2 force = force_body2_on_body1(tab[i], tab[j]);

                total_force.x += force.x;
                total_force.y += force.y;
            }
        }

        Vector2 acceleration = force_to_acceleration(total_force, tab[i]);

        acceleration_to_velocity(acceleration, &tab[i], dt);
        velocity_to_position(&tab[i], dt);
    }

    // Update all bodies and their trajectory histories
    for (int i = 0; i < nbe; i++) {
        tab[i].position = tab[i].position_tp1;
        tab[i].velocity = tab[i].velocity_tp1;

        for (int j = MAX_PREVIOUS - 1; j > 0; j--) {
            tab[i].previous_positions[j] =
                tab[i].previous_positions[j - 1];
        }

        tab[i].previous_positions[0] = tab[i].position;
    }
}