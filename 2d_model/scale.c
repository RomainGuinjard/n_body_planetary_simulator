#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "raylib.h"
#include "scale.h"
#include "rendering.h"


void HandleTextInput(Vector2 mouse, textbox_t* Textbox, char *activeText, int *activeCount) {
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Textbox->edit = CheckCollisionPointRec(mouse, Textbox->rec);
    }
    
    if (Textbox->edit) {
        activeText = Textbox->textValue;
        activeCount = &Textbox->count;
    }

    if (activeText != NULL){
        int key = GetCharPressed();
        while (key > 0){
            if (((key >= '0' && key <= '9') || key == '.' || key == '-' || key == '+' || key == 'e' || key == 'E') && *activeCount < 31)
            {
                activeText[*activeCount] = (char)key;
                (*activeCount)++;
                activeText[*activeCount] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            (*activeCount)--;
            if (*activeCount < 0) *activeCount = 0;
            activeText[*activeCount] = '\0';
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            Textbox->value = atof(Textbox->textValue);
            Textbox->edit = false;
        }
    }
}
