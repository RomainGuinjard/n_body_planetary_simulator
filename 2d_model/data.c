#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "raylib.h"
#include "data.h"


/*Function to read data from a file where the first line is a number indicating how many planets we 
are considering for the simulation. The next lines are the data for each planet: name, mass, position x, 
position y, velocity x, velocity y, radius.*/
body_t* collecting_data(FILE* f, int* nbe) {

    if (fscanf(f, "%d", nbe) != 1 || *nbe <= 0) {
        fprintf(stderr, "Invalid number of bodies\n");
        exit(EXIT_FAILURE);
    }

    body_t* syst = malloc(sizeof(body_t) * (*nbe));

    if (syst == NULL) {
        fprintf(stderr, "Allocation error in collecting_data\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *nbe; i++) {

        char name[100];
        int r, g, b, a;
        float mass, posx, posy, velx, vely, radius;

        int read_count = fscanf(f,
            "%99s %f %f %f %f %f %f %d %d %d %d",
            name, &mass,
            &posx, &posy,
            &velx, &vely,
            &radius,
            &r, &g, &b, &a);

        if (read_count != 11) {
            fprintf(stderr, "Invalid input format for body %d\n", i + 1);
            exit(EXIT_FAILURE);
        }

        strcpy(syst[i].label, name);

        syst[i].mass = mass;
        syst[i].radius = radius;
        syst[i].color = (Color){r, g, b, a};

        syst[i].position = (Vector2){posx, posy};
        syst[i].velocity = (Vector2){velx, vely};

        syst[i].position_tp1 = syst[i].position;
        syst[i].velocity_tp1 = syst[i].velocity;

        syst[i].previous_positions =
            malloc(sizeof(Vector2) * MAX_PREVIOUS);

        if (syst[i].previous_positions == NULL) {
            fprintf(stderr, "Allocation error for previous_positions\n");
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < MAX_PREVIOUS; j++) {
            syst[i].previous_positions[j] = syst[i].position;
        }
    }

    fclose(f);

    return syst;
}

static float scale_coordinate(float value, float AU,
                              float dist_scale, ScaleType scaleDist) {

    float sign = 1.0f;
    float absolute_value;

    if (value < 0.0f) {
        sign = -1.0f;
        absolute_value = -value;
    }
    else {
        absolute_value = value;
    }

    if (AU <= 0.0f) {
        AU = 1.0f;
    }

    if (scaleDist == SCALE_LOG) {
        return sign * log10f(1.0f + absolute_value / AU) * dist_scale;
    }
    else if (scaleDist == SCALE_SQRT) {
        return sign * sqrtf(absolute_value / AU) * dist_scale;
    }

    return value / AU * dist_scale;
}

void virtual(float* radius, Vector2* position,
             ScaleType scaleDist, ScaleType scaleRad,
             float radius_scale, float dist_scale,
             float original_radius, Vector2 original_position,
             float AU) {

    position->x = scale_coordinate(
        original_position.x, AU, dist_scale, scaleDist);

    position->y = scale_coordinate(
        original_position.y, AU, dist_scale, scaleDist);

    if (scaleRad == SCALE_LOG) {
        *radius = log10f(1.0f + original_radius) * radius_scale;
    }
    else if (scaleRad == SCALE_SQRT) {
        *radius = sqrtf(original_radius) * radius_scale;
    }
    else {
        *radius = original_radius * radius_scale;
    }
}



float distancemax(body_t* tab, int nbe){
    float maxDist = 0.0f;
    for (int i = 0; i < nbe; i++)
    {
        float d = sqrtf(tab[i].position.x * tab[i].position.x +
                    tab[i].position.y * tab[i].position.y);

        if (d > maxDist)
            maxDist = d;
    }
    return maxDist;
}