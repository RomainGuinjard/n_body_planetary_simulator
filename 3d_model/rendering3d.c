#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "rendering3d.h"

void DrawButton(button_t Button) {
    int x = Button.rec.x;
    int y = Button.rec.y;

    DrawRectangleRec(Button.rec, LIGHTGRAY);
    DrawRectangleLinesEx(Button.rec, 2, BLACK);

    if (Button.var) {
        DrawText(Button.textOn, x + 10, y + 12, 16, BLACK);
    }
    else {
        DrawText(Button.textOff, x + 10, y + 12, 16, BLACK);
    }
}

void HandleClickOnButton(Vector2 mouse, button_t* Button) {
    if (CheckCollisionPointRec(mouse, Button->rec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Button->var = !Button->var;
    }

    if (IsKeyPressed(Button->key)) {
        Button->var = !Button->var;
    }
}

void DrawTextBox(textbox_t Box) {
    int valueFontSize = 14;

    DrawText(Box.descriptionText, Box.rec.x, Box.rec.y - 25, 16, BLACK);
    DrawRectangleLinesEx(Box.rec, 2, BLACK);
    DrawRectangleRec((Rectangle){Box.rec.x + 2, Box.rec.y + 2, Box.rec.width - 4, Box.rec.height - 4}, RAYWHITE);
    DrawText(Box.textValue, Box.rec.x + 6, Box.rec.y + 7, valueFontSize, BLACK);

    if (Box.edit) {
        DrawRectangleLinesEx(Box.rec, 2, RED);
    }
}

void HandleTextInput(Vector2 mouse, textbox_t* Textbox) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Textbox->edit = CheckCollisionPointRec(mouse, Textbox->rec);
    }

    if (Textbox->edit) {
        int key = GetCharPressed();

        while (key > 0) {
            if (((key >= '0' && key <= '9') ||
                 key == '.' || key == '-' || key == '+' ||
                 key == 'e' || key == 'E') && Textbox->count < 99) {
                Textbox->textValue[Textbox->count] = (char)key;
                Textbox->count++;
                Textbox->textValue[Textbox->count] = '\0';
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (Textbox->count > 0) {
                Textbox->count--;
                Textbox->textValue[Textbox->count] = '\0';
            }
        }

        if (IsKeyPressed(KEY_ENTER)) {
            Textbox->value = atof(Textbox->textValue);
            Textbox->edit = false;
        }
    }
}

void draw_trails_3d(body_t body,
                    int previous,
                    ScaleType scaledist,
                    ScaleType scalerad,
                    float radius_scale,
                    float dist_scale,
                    float AU) {
    if (previous > MAX_PREVIOUS) {
        previous = MAX_PREVIOUS;
    }

    if (previous < 2) {
        return;
    }

    for (int j = 0; j < previous - 1; j++) {
        Vector3 p1;
        Vector3 p2;
        float uselessRadius;

        virtual3d(&uselessRadius, &p1, scaledist, scalerad,
                  radius_scale, dist_scale,
                  body.radius, body.previous_positions[j], AU);

        virtual3d(&uselessRadius, &p2, scaledist, scalerad,
                  radius_scale, dist_scale,
                  body.radius, body.previous_positions[j + 1], AU);

        Color trailColor = body.color;
        trailColor.a = (unsigned char)(255.0f * (previous - j) / previous);

        DrawLine3D(p1, p2, trailColor);
    }
}

void drawVelocity3D(Vector3 startPosition, Vector3 velocity, Color color, float size, float arrowScale, float radius) {
    /*
    This function draws the arrow representing the velocity of a body
    */
    Vector3 direction = velocity;
    float n = vector_norm3d(direction);
    if (n <= 0.0f || !isfinite(n)) {
        return;
    }
    direction.x = direction.x / n;
    direction.y = direction.y / n;
    direction.z = direction.z / n;

    Vector3 endPosition;
    endPosition.x = startPosition.x + direction.x * size * n;
    endPosition.y = startPosition.y + direction.y * size * n;
    endPosition.z = startPosition.z + direction.z * size * n;

    Vector3 ArrowEnd;
    ArrowEnd.x = endPosition.x + direction.x * size * arrowScale;
    ArrowEnd.y = endPosition.y + direction.y * size * arrowScale;
    ArrowEnd.z = endPosition.z + direction.z * size * arrowScale;

    DrawCylinderEx(startPosition, endPosition, radius, radius, 12, color);
    DrawCylinderEx(endPosition, ArrowEnd, radius * arrowScale * 2, 0.1f, 12, color);
}