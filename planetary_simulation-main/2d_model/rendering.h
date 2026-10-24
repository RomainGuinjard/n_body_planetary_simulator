#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "physics.h"

#ifndef RENDERING_H
#define RENDERING_H

typedef struct button {
    Rectangle rec;
    bool var;
    char textOn[100];
    char textOff[100];
    int key;
} button_t;

typedef struct textbox {
    bool edit;
    char textValue[32];
    int count;
    float value;
    Rectangle rec;
    char descriptionText[100];
} textbox_t;

void HandleClickOnButton(Vector2 mouse, button_t* Button);

void DrawButton(button_t Button);

void DrawTextBox(textbox_t Box);

void CamInitialization(Camera2D* camera, int* targetIndex, bool* centered, body_t* tab);

void drawVelocity(Vector2 startPosition, Vector2 velocity, Color color, float size, float arrowScale, float thick);
#endif