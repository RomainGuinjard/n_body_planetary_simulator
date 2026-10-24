#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "data3d.h"

float scale_coordinate(float value, float AU, float dist_scale, ScaleType scaleDist) {
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

body_t* collecting_data(FILE* f, int* nbe) {
    if (fscanf(f, "%d", nbe) != 1) {
        printf("Erreur lecture du nombre de corps\n");
        exit(1);
    }

    body_t* syst = malloc(sizeof(body_t) * (*nbe));
    if (syst == NULL) {
        printf("Erreur malloc collecting_data\n");
        exit(1);
    }

    for (int i = 0; i < *nbe; i++) {
        char name[100];
        int r, g, b, a;
        float mass, posx, posy, posz, velx, vely, velz, radius;

        int read_count = fscanf(f, "%s %f %f %f %f %f %f %f %f %d %d %d %d",
                               name,
                               &mass,
                               &posx, &posy, &posz,
                               &velx, &vely, &velz,
                               &radius,
                               &r, &g, &b, &a);

        if (read_count != 13) {
            printf("Erreur lecture ligne %d : format 3D attendu\n", i + 1);
            printf("Format : nom masse posx posy posz velx vely velz rayon r g b a\n");
            exit(1);
        }

        strcpy(syst[i].label, name);
        syst[i].mass = mass;
        syst[i].radius = radius;
        syst[i].color = (Color){r, g, b, a};

        syst[i].position = (Vector3){posx, posy, posz};
        syst[i].velocity = (Vector3){velx, vely, velz};
        syst[i].position_tp1 = syst[i].position;
        syst[i].velocity_tp1 = syst[i].velocity;

        syst[i].previous_positions = malloc(sizeof(Vector3) * MAX_PREVIOUS);
        if (syst[i].previous_positions == NULL) {
            printf("Erreur malloc previous_positions\n");
            exit(1);
        }

        for (int j = 0; j < MAX_PREVIOUS; j++) {
            syst[i].previous_positions[j] = syst[i].position;
        }
    }

    fclose(f);
    return syst;
}

void virtual3d(float* radius,
               Vector3* position,
               ScaleType scaleDist,
               ScaleType scaleRad,
               float radius_scale,
               float dist_scale,
               float original_radius,
               Vector3 original_position,
               float AU) {
    position->x = scale_coordinate(original_position.x, AU, dist_scale, scaleDist);
    position->y = scale_coordinate(original_position.y, AU, dist_scale, scaleDist);
    position->z = scale_coordinate(original_position.z, AU, dist_scale, scaleDist);

    if (scaleRad == SCALE_LOG) {
        *radius = log10f(1.0f + original_radius) * radius_scale;
    }
    else if (scaleRad == SCALE_SQRT) {
        *radius = sqrtf(original_radius) * radius_scale;
    }
    else {
        *radius = original_radius * radius_scale;
    }

    if (*radius < 0.1f) {
        *radius = 0.1f;
    }
}

float distancemax(body_t* tab, int nbe) {
    float maxDist = 0.0f;

    for (int i = 0; i < nbe; i++) {
        float d = vector_norm3d(tab[i].position);

        if (d > maxDist) {
            maxDist = d;
        }
    }

    if (maxDist <= 0.0f) {
        maxDist = 1.0f;
    }

    return maxDist;
}
