#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "raylib.h"
#include "data.h"


/*Function to read data from a file where the first line is a number indicating how many planets we 
are considering for the simulation. The next lines are the data for each planet: name, mass, position x, 
position y, velocity x, velocity y, radius.*/
body_t* collecting_data(FILE *f,int* nbe){

    fscanf(f,"%d",nbe);

    body_t* syst=malloc(sizeof(body_t)*(*nbe));

    for (int i=0; i<*nbe; i++) {
        char name[100];
        int r,g,b,a;
        float mass, posx, posy, velx, vely, radius;
        fscanf(f,"%s %f %f %f %f %f %f %d %d %d %d", name, &mass, &posx, &posy, &velx, &vely, &radius, &r, &g, &b, &a);
        strcpy(syst[i].label,name); 
        syst[i].mass = mass;
        syst[i].position.x = posx;
        syst[i].position.y = posy;
        syst[i].velocity.x = velx;
        syst[i].velocity.y = vely;
        syst[i].radius = radius;
        syst[i].color = (Color){r, g, b, a};
        Vector2* previous_pos_init = malloc(sizeof(Vector2)*MAX_PREVIOUS);
        for (int j=0; j<MAX_PREVIOUS; j++) {
            previous_pos_init[j] = (Vector2){posx, posy};
        }
        syst[i].previous_positions = previous_pos_init;
    }
    fclose(f);
    return syst;
}

void virtual(float* radius, Vector2* position,
             ScaleType scaleDist, ScaleType scaleRad,
             float radius_scale, float dist_scale,
             float original_radius, Vector2 original_position, float AU)
{
    if (scaleDist == SCALE_LOG)
    {
        position->x = log10f(1 + original_position.x / AU) * dist_scale;
        position->y = log10f(1 + original_position.y / AU) * dist_scale;
    }
    else if (scaleDist == SCALE_SQRT)
    {
        position->x = sqrtf(original_position.x / AU) * dist_scale;
        position->y = sqrtf(original_position.y / AU) * dist_scale;
    }
    else
    {
        position->x = original_position.x / AU * dist_scale;
        position->y = original_position.y / AU * dist_scale;
    }

    if (scaleRad == SCALE_LOG)
    {
        *radius = log10f(1 + original_radius) * radius_scale;
    }
    else if (scaleRad == SCALE_SQRT)
    {
        *radius = sqrtf(original_radius) * radius_scale;
    }
    else
    {
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