#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "tinyfiledialogs.h"
#include "raylib.h"
#include "physics3d.h"
#include "data3d.h"
#include "rendering3d.h"
#include "scale3d.h"
#include "print_body3d.h"
#include "camera_utils3d.h"



int main(int argc, char *argv[]) {
    const char *filters[] = {"*.txt"}; // to filter the files to load

    int screenWidth = 1800;
    int screenHeight = 1000;

    InitWindow(screenWidth, screenHeight, "planetary_simulation_3d");
    SetTargetFPS(60);

    Rectangle info = {GetScreenWidth() - 650, 0, 350, GetScreenHeight()};
    Rectangle info2 = {GetScreenWidth() - 300, 0, 300, GetScreenHeight()};
    Rectangle display = {0, 0, GetScreenWidth() - 650, GetScreenHeight()};

    button_t pauseButton = {(Rectangle){30, 10, 120, 40}, false, "Resume", "Pause", KEY_SPACE};
    button_t trailsButton = {(Rectangle){170, 10, 160, 40}, true, "Remove trails", "Draw trails", KEY_T};
    button_t vectorsButton = {(Rectangle){690, 10, 160, 40}, true, "Remove vectors", "Draw vectors", KEY_V};
    button_t resetSimButton = {(Rectangle){350, 10, 160, 40}, false, "Reset simulation", "Reset simulation", KEY_R};
    button_t resetCamButton = {(Rectangle){530, 10, 150, 40}, false, "Reset camera", "Reset camera", KEY_C};
    button_t loadButton = {(Rectangle){860, 10, 150, 40}, false, "Load new file", "Load new file", KEY_L};

    textbox_t Radius = {false, "0.003", 5, 0.003f,
                        (Rectangle){info2.x + 20, 300, 180, 35},
                        "Radius scale correction:"};

    textbox_t Dist = {false, "390.0", 5, 390.0f,
                      (Rectangle){info2.x + 20, 390, 180, 35},
                      "Distance scale correction:"};

    textbox_t Dt = {false, "10000.0", 7, 10000.0f,
                    (Rectangle){info2.x + 20, 480, 180, 35},
                    "dt :"};

    textbox_t Previous = {false, "100.0", 5, 100.0f,
                          (Rectangle){info2.x + 20, 570, 180, 35},
                          "number of previous values :"};

    ScaleType scalerad = SCALE_SQRT;
    ScaleType scaledist = SCALE_LINEAR;

    Rectangle radlinearBtn = {screenWidth - 290, 50, 16, 16};
    Rectangle radlogBtn = {screenWidth - 290, 80, 16, 16};
    Rectangle radsqrtBtn = {screenWidth - 290, 110, 16, 16};

    Rectangle distlinearBtn = {screenWidth - 290, 170, 16, 16};
    Rectangle distlogBtn = {screenWidth - 290, 200, 16, 16};
    Rectangle distsqrtBtn = {screenWidth - 290, 230, 16, 16};

    char current_file[256] = "systeme3d.txt";
    FILE* f = fopen(current_file, "r");
    if (f == NULL) {
        printf("Erreur fichier : %s\n", current_file);
        return 1;
    }

    int nbe;
    body_t* tab = collecting_data(f, &nbe);

    textbox_t* bodyBoxes = NULL;
    int bodyBoxNbe = 0;
    InitBodyTextBoxes(&bodyBoxes, &bodyBoxNbe, tab, nbe);

    Camera3D camera;
    reset_camera(&camera);

    float scrollOffset = 0.0f;
    int selectedBody = -1;
    int followSelectedBody = 0;

    while (!WindowShouldClose()) {
        info.x = GetScreenWidth() - 650;
        info.height = GetScreenHeight();
        info2.x = GetScreenWidth() - 300;
        info2.height = GetScreenHeight();
        display.width = GetScreenWidth() - 650;
        display.height = GetScreenHeight();

        Radius.rec.x = info2.x + 20;
        Dist.rec.x = info2.x + 20;
        Dt.rec.x = info2.x + 20;
        Previous.rec.x = info2.x + 20;

        radlinearBtn.x = GetScreenWidth() - 290;
        radlogBtn.x = GetScreenWidth() - 290;
        radsqrtBtn.x = GetScreenWidth() - 290;
        distlinearBtn.x = GetScreenWidth() - 290;
        distlogBtn.x = GetScreenWidth() - 290;
        distsqrtBtn.x = GetScreenWidth() - 290;

        Vector2 mouse = GetMousePosition();
        float wheel = GetMouseWheelMove();

        HandleClickOnButton(mouse, &pauseButton);
        HandleClickOnButton(mouse, &trailsButton);
        HandleClickOnButton(mouse, &vectorsButton);
        HandleClickOnButton(mouse, &resetSimButton);
        HandleClickOnButton(mouse, &resetCamButton);
        HandleClickOnButton(mouse, &loadButton);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, radlinearBtn)) {
                scalerad = SCALE_LINEAR;
            }
            if (CheckCollisionPointRec(mouse, radlogBtn)) {
                scalerad = SCALE_LOG;
            }
            if (CheckCollisionPointRec(mouse, radsqrtBtn)) {
                scalerad = SCALE_SQRT;
            }
            if (CheckCollisionPointRec(mouse, distlinearBtn)) {
                scaledist = SCALE_LINEAR;
            }
            if (CheckCollisionPointRec(mouse, distlogBtn)) {
                scaledist = SCALE_LOG;
            }
            if (CheckCollisionPointRec(mouse, distsqrtBtn)) {
                scaledist = SCALE_SQRT;
            }
        }

        HandleTextInput(mouse, &Radius);
        HandleTextInput(mouse, &Dist);
        HandleTextInput(mouse, &Dt);
        HandleTextInput(mouse, &Previous);

        if (CheckCollisionPointRec(mouse, info)) {
            scrollOffset -= wheel * 20.0f;
        }
        else if (CheckCollisionPointRec(mouse, display) && wheel != 0.0f) {
            camera.position.x -= camera.target.x;
            camera.position.y -= camera.target.y;
            camera.position.z -= camera.target.z;

            float zoomFactor = 1.0f - wheel * 0.08f;
            if (zoomFactor < 0.2f) {
                zoomFactor = 0.2f;
            }

            camera.position.x = camera.target.x + camera.position.x * zoomFactor;
            camera.position.y = camera.target.y + camera.position.y * zoomFactor;
            camera.position.z = camera.target.z + camera.position.z * zoomFactor;
        }

        UpdateBodyTextBoxes(mouse, &bodyBoxes, &bodyBoxNbe, tab, nbe, info, scrollOffset);

        if (resetSimButton.var) {
            FILE* file_reset = fopen(current_file, "r");
            if (file_reset != NULL) {
                free_bodies(tab, nbe);
                tab = collecting_data(file_reset, &nbe);
                InitBodyTextBoxes(&bodyBoxes, &bodyBoxNbe, tab, nbe);
            }
            resetSimButton.var = false;
        }

        if (loadButton.var) {
            const char *path = tinyfd_openFileDialog(
                    "Choose a file",        // title
                    current_file,           // initial file
                    1,                      // filters number
                    filters,                // filters
                    "Simulations files",    // filters label
                    0                       // 0 = simple selection
                );            
            if (path != NULL) {
                strncpy(current_file, path, sizeof(current_file) - 1);
                resetSimButton.var = true;
            }
            loadButton.var = false;
        }

        if (resetCamButton.var) {
            reset_camera(&camera);
            resetCamButton.var = false;
        }

        if (CheckCollisionPointRec(mouse, display) && !followSelectedBody) {
            UpdateCamera(&camera, CAMERA_FREE);
        }

        float maxDist = distancemax(tab, nbe);
        float AU = maxDist / 30.0f;

        if (selectedBody >= nbe) {
            selectedBody = -1;
            followSelectedBody = 0;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, display)) {
            int clickedBody = select_body_3d(mouse, camera, tab, nbe,
                                            scaledist, scalerad,
                                            Radius.value, Dist.value, AU);

            if (clickedBody != -1) {
                selectedBody = clickedBody;
                followSelectedBody = 1;
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, info)) {
            int clickedBody = SelectBodyFromInfoPanel(mouse, info, scrollOffset, nbe);

            if (clickedBody != -1) {
                selectedBody = clickedBody;
                followSelectedBody = 1;
            }
        }

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            selectedBody = -1;
            followSelectedBody = 0;
        }

        if (!pauseButton.var) {
            update_physics(tab, nbe, Dt.value);
            handle_collisions(&tab, &nbe);
        }

        if (selectedBody >= nbe) {
            selectedBody = -1;
            followSelectedBody = 0;
        }

        if (followSelectedBody && selectedBody != -1) {
            follow_body_3d(&camera, tab[selectedBody],
                        scaledist, scalerad,
                        Radius.value, Dist.value, AU);
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginScissorMode((int)display.x,
                            (int)display.y,
                            (int)display.width,
                            (int)display.height);

                BeginMode3D(camera);

                    // 1. D'abord le maillage
                    DrawGrid(200, 20.0f);

                    // 2. Ensuite les traînées
                    if (trailsButton.var) {
                        for (int i = 0; i < nbe; i++) {
                            draw_trails_3d(tab[i], (int)Previous.value,
                                        scaledist, scalerad,
                                        Radius.value, Dist.value, AU);
                        }
                    }

                    // 3. Enfin les planètes
                    for (int i = 0; i < nbe; i++) {
                        Vector3 screenPos;
                        float screenRadius;

                        virtual3d(&screenRadius, &screenPos,
                                scaledist, scalerad,
                                Radius.value, Dist.value,
                                tab[i].radius, tab[i].position, AU);

                        DrawSphere(screenPos, screenRadius, tab[i].color);

                        if (vectorsButton.var) {
                            float uselessRadius;

                            Vector3 futurePosition = {
                                tab[i].position.x + tab[i].velocity.x * Dt.value,
                                tab[i].position.y + tab[i].velocity.y * Dt.value,
                                tab[i].position.z + tab[i].velocity.z * Dt.value,
                            };

                            Vector3 futureScreenPos;

                            virtual3d(&uselessRadius, &futureScreenPos,
                                        scaledist, scalerad,
                                        Radius.value, Dist.value,
                                        tab[i].radius, futurePosition, AU);

                            Vector3 screenVelocity = {
                                futureScreenPos.x - screenPos.x,
                                futureScreenPos.y - screenPos.y,
                                futureScreenPos.z - screenPos.z
                            };

                            drawVelocity3D(screenPos, screenVelocity, tab[i].color, 13.0f, 1.0f, 2.0f);

                        }
                    }

                EndMode3D();
            EndScissorMode();

            DrawRectangleRec(info2, LIGHTGRAY);
            DrawRectangleLinesEx(info2, 1, DARKGRAY);

            DrawScaleChoice(radlinearBtn, radlogBtn, radsqrtBtn, scalerad, "Radius scale:");
            DrawScaleChoice(distlinearBtn, distlogBtn, distsqrtBtn, scaledist, "Distance scale:");

            DrawTextBox(Radius);
            DrawTextBox(Dist);
            DrawTextBox(Dt);
            DrawTextBox(Previous);

            DrawText(TextFormat("radius_scale = %.5f", Radius.value), info2.x + 20, 690, 14, BLACK);
            DrawText(TextFormat("dist_scale = %.2f", Dist.value), info2.x + 20, 720, 14, BLACK);
            DrawText(TextFormat("dt = %.2f", Dt.value), info2.x + 20, 750, 14, BLACK);
            DrawText(TextFormat("previous = %.0f", Previous.value), info2.x + 20, 780, 14, BLACK);

            DrawBodyEditableInfo(info, &scrollOffset, bodyBoxes, tab, nbe);

            DrawButton(pauseButton);
            DrawButton(trailsButton);
            DrawButton(resetSimButton);
            DrawButton(resetCamButton);
            DrawButton(vectorsButton);
            DrawButton(loadButton);

            if (selectedBody != -1) {
                DrawText(TextFormat("Following: %s   |   right click = stop following",
                                    tab[selectedBody].label),
                        20, GetScreenHeight() - 30, 16, DARKGRAY);
            }
            else {
                DrawText("3D controls: click planet or info panel = follow, right click = stop",
                        20, GetScreenHeight() - 30, 16, DARKGRAY);
            }
        EndDrawing();
    }

    free(bodyBoxes);
    free_bodies(tab, nbe);

    CloseWindow();

    return 0;
}
