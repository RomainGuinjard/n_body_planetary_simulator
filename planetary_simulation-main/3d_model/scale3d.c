#include "scale3d.h"

void DrawScaleChoice(Rectangle linearBtn,
                     Rectangle logBtn,
                     Rectangle sqrtBtn,
                     ScaleType currentScale,
                     char *title) {
    DrawText(title, linearBtn.x, linearBtn.y - 25, 16, BLACK);

    DrawRectangleLinesEx(linearBtn, 2, BLACK);
    DrawText("Linear", linearBtn.x + 25, linearBtn.y - 2, 16, BLACK);

    DrawRectangleLinesEx(logBtn, 2, BLACK);
    DrawText("Log", logBtn.x + 25, logBtn.y - 2, 16, BLACK);

    DrawRectangleLinesEx(sqrtBtn, 2, BLACK);
    DrawText("Sqrt", sqrtBtn.x + 25, sqrtBtn.y - 2, 16, BLACK);

    if (currentScale == SCALE_LINEAR) {
        DrawRectangleRec((Rectangle){linearBtn.x + 4, linearBtn.y + 4, linearBtn.width - 8, linearBtn.height - 8}, BLACK);
    }
    else if (currentScale == SCALE_LOG) {
        DrawRectangleRec((Rectangle){logBtn.x + 4, logBtn.y + 4, logBtn.width - 8, logBtn.height - 8}, BLACK);
    }
    else if (currentScale == SCALE_SQRT) {
        DrawRectangleRec((Rectangle){sqrtBtn.x + 4, sqrtBtn.y + 4, sqrtBtn.width - 8, sqrtBtn.height - 8}, BLACK);
    }
}
