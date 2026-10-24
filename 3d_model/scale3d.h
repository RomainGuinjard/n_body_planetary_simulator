#ifndef SCALE3D_H
#define SCALE3D_H

#include "raylib.h"
#include "data3d.h"

void DrawScaleChoice(Rectangle linearBtn,
                     Rectangle logBtn,
                     Rectangle sqrtBtn,
                     ScaleType currentScale,
                     char *title);

#endif
