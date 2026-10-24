#ifndef RENDERING3D_H
#define RENDERING3D_H

#include "raylib.h"
#include "data3d.h"
#include "scale3d.h"

typedef struct button {
    Rectangle rec;
    bool var;
    char textOn[100];
    char textOff[100];
    KeyboardKey key;
} button_t;

typedef struct textbox {
    bool edit;
    char textValue[100];
    int count;
    float value;
    Rectangle rec;
    char descriptionText[100];
} textbox_t;

void DrawButton(button_t Button);

void HandleClickOnButton(Vector2 mouse, button_t* Button);

void DrawTextBox(textbox_t Box);

void HandleTextInput(Vector2 mouse, textbox_t* Textbox);

void draw_trails_3d(body_t body,
                    int previous,
                    ScaleType scaledist,
                    ScaleType scalerad,
                    float radius_scale,
                    float dist_scale,
                    float AU);

void drawVelocity3D(Vector3 startPosition, Vector3 velocity, Color color, float size, float arrowScale, float radius);

#endif
