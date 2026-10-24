#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "physics3d.h"

float vector_norm3d(Vector3 vector) {
    return sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

Vector3 force_body2_on_body1(body_t body1, body_t body2) {
    double dx = (double)body2.position.x - (double)body1.position.x;
    double dy = (double)body2.position.y - (double)body1.position.y;
    double dz = (double)body2.position.z - (double)body1.position.z;

    double distance = sqrt(dx * dx + dy * dy + dz * dz);

    if (distance <= 0.0) {
        return (Vector3){0.0f, 0.0f, 0.0f};
    }

    double coefficient = G * (double)body1.mass * (double)body2.mass
                       / (distance * distance * distance);

    Vector3 force;
    force.x = (float)(coefficient * dx);
    force.y = (float)(coefficient * dy);
    force.z = (float)(coefficient * dz);

    return force;
}

Vector3 force_to_acceleration(Vector3 force, body_t body) {
    Vector3 acceleration;

    if (body.mass <= 0.0f) {
        return (Vector3){0.0f, 0.0f, 0.0f};
    }

    acceleration.x = force.x / body.mass;
    acceleration.y = force.y / body.mass;
    acceleration.z = force.z / body.mass;

    return acceleration;
}

void acceleration_to_velocity(Vector3 acceleration, body_t* body, float dt) {
    body->velocity_tp1.x = body->velocity.x + acceleration.x * dt;
    body->velocity_tp1.y = body->velocity.y + acceleration.y * dt;
    body->velocity_tp1.z = body->velocity.z + acceleration.z * dt;
}

void velocity_to_position(body_t* body, float dt) {
    body->position_tp1.x = body->position.x + body->velocity_tp1.x * dt;
    body->position_tp1.y = body->position.y + body->velocity_tp1.y * dt;
    body->position_tp1.z = body->position.z + body->velocity_tp1.z * dt;
}

bool collision_occurred(body_t body1, body_t body2) {
    Vector3 difference;
    difference.x = body1.position.x - body2.position.x;
    difference.y = body1.position.y - body2.position.y;
    difference.z = body1.position.z - body2.position.z;

    float distance = vector_norm3d(difference);

    return distance < body1.radius + body2.radius;
}

body_t collision_new_body(body_t body1, body_t body2) {
    body_t new_body;

    snprintf(new_body.label, sizeof(new_body.label), "%.40s_%.40s",
             body1.label, body2.label);

    double mass1 = (double)body1.mass;
    double mass2 = (double)body2.mass;
    double total_mass = mass1 + mass2;

    if (total_mass <= 0.0) {
        total_mass = 1.0;
    }

    new_body.mass = (float)total_mass;

    new_body.radius = powf(powf(body1.radius, 3.0f)
                         + powf(body2.radius, 3.0f),
                           1.0f / 3.0f);

    new_body.color.r = (unsigned char)(((double)body1.color.r * mass1 + (double)body2.color.r * mass2) / total_mass);
    new_body.color.g = (unsigned char)(((double)body1.color.g * mass1 + (double)body2.color.g * mass2) / total_mass);
    new_body.color.b = (unsigned char)(((double)body1.color.b * mass1 + (double)body2.color.b * mass2) / total_mass);
    new_body.color.a = 255;

    new_body.position.x = (float)(((double)body1.position.x * mass1 + (double)body2.position.x * mass2) / total_mass);
    new_body.position.y = (float)(((double)body1.position.y * mass1 + (double)body2.position.y * mass2) / total_mass);
    new_body.position.z = (float)(((double)body1.position.z * mass1 + (double)body2.position.z * mass2) / total_mass);

    new_body.velocity.x = (float)(((double)body1.velocity.x * mass1 + (double)body2.velocity.x * mass2) / total_mass);
    new_body.velocity.y = (float)(((double)body1.velocity.y * mass1 + (double)body2.velocity.y * mass2) / total_mass);
    new_body.velocity.z = (float)(((double)body1.velocity.z * mass1 + (double)body2.velocity.z * mass2) / total_mass);

    new_body.position_tp1 = new_body.position;
    new_body.velocity_tp1 = new_body.velocity;

    new_body.previous_positions = malloc(sizeof(Vector3) * MAX_PREVIOUS);
    if (new_body.previous_positions == NULL) {
        printf("Erreur malloc previous_positions dans collision_new_body\n");
        exit(1);
    }

    for (int k = 0; k < MAX_PREVIOUS; k++) {
        new_body.previous_positions[k] = new_body.position;
    }

    return new_body;
}

void handle_collisions(body_t** tab, int* nbe) {
    int i = 0;

    while (i < *nbe) {
        int j = i + 1;

        while (j < *nbe) {
            if (collision_occurred((*tab)[i], (*tab)[j])) {
                body_t new_body = collision_new_body((*tab)[i], (*tab)[j]);

                free((*tab)[i].previous_positions);
                free((*tab)[j].previous_positions);

                (*tab)[i] = new_body;

                for (int k = j; k < *nbe - 1; k++) {
                    (*tab)[k] = (*tab)[k + 1];
                }

                *nbe = *nbe - 1;

                body_t* smaller_tab = malloc(sizeof(body_t) * (*nbe));
                if (smaller_tab == NULL) {
                    printf("Erreur malloc handle_collisions\n");
                    exit(1);
                }

                for (int k = 0; k < *nbe; k++) {
                    smaller_tab[k] = (*tab)[k];
                }

                free(*tab);
                *tab = smaller_tab;
            }
            else {
                j++;
            }
        }

        i++;
    }
}

void free_bodies(body_t* tab, int nbe) {
    if (tab == NULL) {
        return;
    }

    for (int i = 0; i < nbe; i++) {
        free(tab[i].previous_positions);
    }

    free(tab);
}

void update_physics(body_t* tab, int nbe, float dt) {
    for (int i = 0; i < nbe; i++) {
        Vector3 total_force = {0.0f, 0.0f, 0.0f};

        for (int j = 0; j < nbe; j++) {
            if (i != j) {
                Vector3 force = force_body2_on_body1(tab[i], tab[j]);
                total_force.x += force.x;
                total_force.y += force.y;
                total_force.z += force.z;
            }
        }

        Vector3 acceleration = force_to_acceleration(total_force, tab[i]);
        acceleration_to_velocity(acceleration, &tab[i], dt);
        velocity_to_position(&tab[i], dt);
    }

    for (int i = 0; i < nbe; i++) {
        tab[i].position = tab[i].position_tp1;
        tab[i].velocity = tab[i].velocity_tp1;

        for (int j = MAX_PREVIOUS - 1; j > 0; j--) {
            tab[i].previous_positions[j] = tab[i].previous_positions[j - 1];
        }

        tab[i].previous_positions[0] = tab[i].position;
    }
}
