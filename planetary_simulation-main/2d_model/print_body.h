#ifndef BODY_EDIT_H
#define BODY_EDIT_H

#include "raylib.h"
#include "physics.h"
#include "rendering.h"

#define BODY_FIELD_COUNT 6

#define BODY_MASS 0
#define BODY_POS_X 1
#define BODY_POS_Y 2
#define BODY_VEL_X 3
#define BODY_VEL_Y 4
#define BODY_RADIUS 5

float BodyFieldValue(body_t* body, int field);

void SetBodyFieldValue(body_t* body, int field, float value);

void SetTextBoxFloat(textbox_t* box, float value);

void InitBodyTextBoxes(textbox_t** boxes, int* boxNbe, body_t* tab, int nbe);

void SyncBodyTextBoxes(textbox_t* boxes, body_t* tab, int nbe);

void ApplyBodyTextBoxes(textbox_t* boxes, body_t* tab, int nbe);

void PlaceBodyTextBoxes(textbox_t* boxes, int nbe, Rectangle info, float scrollOffset);

void HandleBodyTextBoxes(Vector2 mouse, textbox_t* boxes, int nbe);

void UpdateBodyTextBoxes(Vector2 mouse,
                         textbox_t** boxes,
                         int* boxNbe,
                         body_t* tab,
                         int nbe,
                         Rectangle info,
                         float scrollOffset);

void DrawBodyEditableInfo(Rectangle info,
                          float* scrollOffset,
                          textbox_t* boxes,
                          body_t* tab,
                          int nbe);

#endif