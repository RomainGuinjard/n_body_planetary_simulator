#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print_body3d.h"

float BodyFieldValue(body_t* body, int field) {
    if (field == BODY_MASS) {
        return body->mass;
    }
    else if (field == BODY_POS_X) {
        return body->position.x;
    }
    else if (field == BODY_POS_Y) {
        return body->position.y;
    }
    else if (field == BODY_POS_Z) {
        return body->position.z;
    }
    else if (field == BODY_VEL_X) {
        return body->velocity.x;
    }
    else if (field == BODY_VEL_Y) {
        return body->velocity.y;
    }
    else if (field == BODY_VEL_Z) {
        return body->velocity.z;
    }
    else if (field == BODY_RADIUS) {
        return body->radius;
    }

    return 0.0f;
}

void SetBodyFieldValue(body_t* body, int field, float value) {
    if (field == BODY_MASS) {
        if (value > 0.0f) {
            body->mass = value;
        }
    }
    else if (field == BODY_POS_X) {
        body->position.x = value;
        body->position_tp1.x = value;
    }
    else if (field == BODY_POS_Y) {
        body->position.y = value;
        body->position_tp1.y = value;
    }
    else if (field == BODY_POS_Z) {
        body->position.z = value;
        body->position_tp1.z = value;
    }
    else if (field == BODY_VEL_X) {
        body->velocity.x = value;
        body->velocity_tp1.x = value;
    }
    else if (field == BODY_VEL_Y) {
        body->velocity.y = value;
        body->velocity_tp1.y = value;
    }
    else if (field == BODY_VEL_Z) {
        body->velocity.z = value;
        body->velocity_tp1.z = value;
    }
    else if (field == BODY_RADIUS) {
        if (value > 0.0f) {
            body->radius = value;
        }
    }
}

void SetTextBoxFloat(textbox_t* box, float value) {
    snprintf(box->textValue, sizeof(box->textValue), "%.6g", value);
    box->count = (int)strlen(box->textValue);
    box->value = value;
}

void InitBodyTextBoxes(textbox_t** boxes, int* boxNbe, body_t* tab, int nbe) {
    free(*boxes);

    *boxes = malloc(nbe * BODY_FIELD_COUNT * sizeof(textbox_t));
    if (*boxes == NULL) {
        printf("Erreur malloc body textboxes\n");
        exit(1);
    }

    *boxNbe = nbe;

    for (int i = 0; i < nbe; i++) {
        for (int field = 0; field < BODY_FIELD_COUNT; field++) {
            textbox_t* box = &(*boxes)[i * BODY_FIELD_COUNT + field];

            box->edit = false;
            box->rec = (Rectangle){0, 0, 120, 24};
            box->descriptionText[0] = '\0';

            SetTextBoxFloat(box, BodyFieldValue(&tab[i], field));
        }
    }
}

void SyncBodyTextBoxes(textbox_t* boxes, body_t* tab, int nbe) {
    for (int i = 0; i < nbe; i++) {
        for (int field = 0; field < BODY_FIELD_COUNT; field++) {
            textbox_t* box = &boxes[i * BODY_FIELD_COUNT + field];

            if (!box->edit) {
                SetTextBoxFloat(box, BodyFieldValue(&tab[i], field));
            }
        }
    }
}

void ApplyBodyTextBoxes(textbox_t* boxes, body_t* tab, int nbe) {
    for (int i = 0; i < nbe; i++) {
        Vector3 oldPosition = tab[i].position;

        for (int field = 0; field < BODY_FIELD_COUNT; field++) {
            textbox_t* box = &boxes[i * BODY_FIELD_COUNT + field];

            if (!box->edit) {
                SetBodyFieldValue(&tab[i], field, box->value);
            }
        }

        if (tab[i].position.x != oldPosition.x ||
            tab[i].position.y != oldPosition.y ||
            tab[i].position.z != oldPosition.z) {
            for (int k = 0; k < MAX_PREVIOUS; k++) {
                tab[i].previous_positions[k] = tab[i].position;
            }
        }
    }
}

void PlaceBodyTextBoxes(textbox_t* boxes, int nbe, Rectangle info, float scrollOffset) {
    int fontSize = 15;
    int lineHeight = fontSize + 9;
    int y = (int)info.y + 10 - (int)scrollOffset;

    for (int i = 0; i < nbe; i++) {
        y += lineHeight;

        boxes[i * BODY_FIELD_COUNT + BODY_MASS].rec =
            (Rectangle){info.x + 100, y - 4, 170, 24};

        y += lineHeight;

        boxes[i * BODY_FIELD_COUNT + BODY_POS_X].rec =
            (Rectangle){info.x + 65, y - 4, 70, 24};
        boxes[i * BODY_FIELD_COUNT + BODY_POS_Y].rec =
            (Rectangle){info.x + 155, y - 4, 70, 24};
        boxes[i * BODY_FIELD_COUNT + BODY_POS_Z].rec =
            (Rectangle){info.x + 245, y - 4, 70, 24};

        y += lineHeight;

        boxes[i * BODY_FIELD_COUNT + BODY_VEL_X].rec =
            (Rectangle){info.x + 65, y - 4, 70, 24};
        boxes[i * BODY_FIELD_COUNT + BODY_VEL_Y].rec =
            (Rectangle){info.x + 155, y - 4, 70, 24};
        boxes[i * BODY_FIELD_COUNT + BODY_VEL_Z].rec =
            (Rectangle){info.x + 245, y - 4, 70, 24};

        y += lineHeight;

        boxes[i * BODY_FIELD_COUNT + BODY_RADIUS].rec =
            (Rectangle){info.x + 100, y - 4, 170, 24};

        y += lineHeight + 12;
    }
}

void HandleBodyTextBoxes(Vector2 mouse, textbox_t* boxes, int nbe) {
    for (int i = 0; i < nbe; i++) {
        for (int field = 0; field < BODY_FIELD_COUNT; field++) {
            HandleTextInput(mouse, &boxes[i * BODY_FIELD_COUNT + field]);
        }
    }
}

void UpdateBodyTextBoxes(Vector2 mouse,
                         textbox_t** boxes,
                         int* boxNbe,
                         body_t* tab,
                         int nbe,
                         Rectangle info,
                         float scrollOffset) {
    if (*boxNbe != nbe) {
        InitBodyTextBoxes(boxes, boxNbe, tab, nbe);
    }

    SyncBodyTextBoxes(*boxes, tab, nbe);
    PlaceBodyTextBoxes(*boxes, nbe, info, scrollOffset);
    HandleBodyTextBoxes(mouse, *boxes, nbe);
    ApplyBodyTextBoxes(*boxes, tab, nbe);
}

void DrawBodyEditableInfo(Rectangle info,
                          float* scrollOffset,
                          textbox_t* boxes,
                          body_t* tab,
                          int nbe) {
    DrawRectangleRec(info, LIGHTGRAY);
    DrawRectangleLinesEx(info, 1, DARKGRAY);

    int fontSize = 15;
    int lineHeight = fontSize + 9;
    int blockHeight = 5 * lineHeight + 12;
    int contentHeight = nbe * blockHeight;

    if (*scrollOffset < 0.0f) {
        *scrollOffset = 0.0f;
    }

    float maxScroll = contentHeight - info.height + 10;

    if (maxScroll < 0.0f) {
        maxScroll = 0.0f;
    }

    if (*scrollOffset > maxScroll) {
        *scrollOffset = maxScroll;
    }

    PlaceBodyTextBoxes(boxes, nbe, info, *scrollOffset);

    BeginScissorMode((int)info.x, (int)info.y, (int)info.width, (int)info.height);

    int y = (int)info.y + 10 - (int)(*scrollOffset);

    for (int i = 0; i < nbe; i++) {
        DrawText(TextFormat("Element : %s", tab[i].label), info.x + 10, y, fontSize, tab[i].color);

        y += lineHeight;

        DrawText("Mass :", info.x + 10, y, fontSize, BLACK);
        DrawTextBox(boxes[i * BODY_FIELD_COUNT + BODY_MASS]);

        y += lineHeight;

        DrawText("Pos x", info.x + 10, y, fontSize, BLACK);
        DrawTextBox(boxes[i * BODY_FIELD_COUNT + BODY_POS_X]);
        DrawText("y", info.x + 140, y, fontSize, BLACK);
        DrawTextBox(boxes[i * BODY_FIELD_COUNT + BODY_POS_Y]);
        DrawText("z", info.x + 230, y, fontSize, BLACK);
        DrawTextBox(boxes[i * BODY_FIELD_COUNT + BODY_POS_Z]);

        y += lineHeight;

        DrawText("Vel x", info.x + 10, y, fontSize, BLACK);
        DrawTextBox(boxes[i * BODY_FIELD_COUNT + BODY_VEL_X]);
        DrawText("y", info.x + 140, y, fontSize, BLACK);
        DrawTextBox(boxes[i * BODY_FIELD_COUNT + BODY_VEL_Y]);
        DrawText("z", info.x + 230, y, fontSize, BLACK);
        DrawTextBox(boxes[i * BODY_FIELD_COUNT + BODY_VEL_Z]);

        y += lineHeight;

        DrawText("Radius :", info.x + 10, y, fontSize, BLACK);
        DrawTextBox(boxes[i * BODY_FIELD_COUNT + BODY_RADIUS]);

        y += lineHeight + 12;
    }

    EndScissorMode();
}

int SelectBodyFromInfoPanel(Vector2 mouse,
                            Rectangle info,
                            float scrollOffset,
                            int nbe) {
    int fontSize = 15;
    int lineHeight = fontSize + 9;
    int blockHeight = 5 * lineHeight + 12;

    if (!CheckCollisionPointRec(mouse, info)) {
        return -1;
    }

    int y = (int)info.y + 10 - (int)scrollOffset;

    for (int i = 0; i < nbe; i++) {
        Rectangle bodyBlock = {
            info.x,
            y,
            info.width,
            blockHeight
        };

        if (CheckCollisionPointRec(mouse, bodyBlock)) {
            return i;
        }

        y += blockHeight;
    }

    return -1;
}

