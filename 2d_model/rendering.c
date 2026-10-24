#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "raylib.h"
#include "rendering.h"
#include "physics.h"

void HandleClickOnButton(Vector2 mouse, button_t* Button) {
    /*
    This function modifies the bool value associated to the button
    when the button is clicked.

    Parameters:
    Vector2 mouse    : position of the mouse on the screen
    button_t* Button : the button we want to handle
    */
    if ((CheckCollisionPointRec(mouse, Button->rec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        || IsKeyPressed(Button->key))

        {Button->var = !Button->var;}
}

void DrawButton(button_t Button) {
    /*
    This function draws the rectangle and the text associated to the button.

    Parameters:
    button_t Button : the button we want to draw
    */
    DrawRectangleRec(Button.rec, LIGHTGRAY);
    DrawRectangleLinesEx(Button.rec, 2, DARKGRAY); // DrawRectangleLinesEx(Rectangle rec, float lineThick, Color color);
    int x, y;
    x = Button.rec.x;
    y = Button.rec.y;
    if (Button.var) DrawText(Button.textOn, x+10, y+12, 18, BLACK);
    else DrawText(Button.textOff, x+10, y+12, 18, BLACK);
}

void DrawTextBox(textbox_t Box) {
    /*
    This function draws the rectangle and the text associated to the textbox.

    Parameters:
    textbox_t Box   : the textbox we want to draw
    */
    DrawText(Box.descriptionText, Box.rec.x, Box.rec.y-30, 20, BLACK);
    DrawRectangleRec(Box.rec, RAYWHITE);
    DrawRectangleLinesEx(Box.rec, 2, Box.edit ? BLUE : DARKGRAY);
    DrawText(Box.textValue, Box.rec.x + 10, Box.rec.y + 8, 14, BLACK);
}

void CamInitialization(Camera2D* camera, int* targetIndex, bool* centered, body_t* tab) {
    /*
    This function initializes the camera an makes it centered on the first object in tab
    */
    camera->offset = (Vector2){(GetScreenWidth() - 600) / 2, GetScreenHeight() / 2};
    *targetIndex = 0;
    camera->target = tab[*targetIndex].position;
    camera->rotation = 0;
    camera->zoom = 1.0f;
    *centered = true;
}

void drawVelocity(Vector2 startPosition, Vector2 velocity, Color color, float size, float arrowScale, float thick) {
    /*
    This function draws the arrow representing the velocity of a body
    */
    Vector2 direction = velocity;
    float n = vector_norm(direction);
    direction.x = direction.x / n;
    direction.y = direction.y / n;

    Vector2 endPosition;
    endPosition.x = startPosition.x + direction.x * size * n;
    endPosition.y = startPosition.y + direction.y * size * n;

    Vector2 ArrowEnd;
    ArrowEnd.x = endPosition.x + direction.x * size * arrowScale;
    ArrowEnd.y = endPosition.y + direction.y * size * arrowScale;

    Vector2 ArrowLeft;
    ArrowLeft.x = endPosition.x - direction.y * size * arrowScale / 2;
    ArrowLeft.y = endPosition.y + direction.x * size * arrowScale / 2;

    Vector2 ArrowRight;
    ArrowRight.x = endPosition.x + direction.y * size * arrowScale / 2;
    ArrowRight.y = endPosition.y - direction.x * size * arrowScale / 2;

    DrawLineEx(startPosition, endPosition, thick, color);
    DrawTriangle(ArrowEnd, ArrowRight, ArrowLeft, color);
}