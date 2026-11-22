#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <math.h>

#include "tinyfiledialogs.h"
#include "raylib.h"
#include "data.h"
#include "scale.h"
#include "rendering.h"
#include "print_body.h"


int main(int argc, char *argv[]){

    //-----------------------------------------------------------------------------
    //      Initialization
    //-----------------------------------------------------------------------------
    const char *filters[] = {"*.txt"}; // to filter the files to load
    int screenWidth = 1800;
    int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "planetary_simulation");
    int currentFps = 60;
    SetTargetFPS(currentFps);
    long int nb_iter = 0;
    
    // Initialization of the info rectangles
    Rectangle info = {GetScreenWidth()-600, 0, 300, GetScreenHeight()};
    float scrollOffset = 0.0f;
    Rectangle info2 = {GetScreenWidth()-300, 0, 300, GetScreenHeight()};
    Rectangle display = {0, 0, GetScreenWidth()-600, GetScreenHeight()};

    // Buttons initialization                          
    button_t pauseButton  = (button_t){(Rectangle){490, 10, 120, 40}, false, "Resume", "Pause", KEY_SPACE};
    button_t trailsButton = (button_t){(Rectangle){620, 10, 160, 40}, true, "Remove trails", "Draw trails", KEY_T};
    button_t vectorsButton = (button_t){(Rectangle){790, 10, 160, 40}, true, "Remove vectors", "Draw vectors", KEY_V};
    button_t resetSimButton = (button_t){(Rectangle){170, 10, 150, 40}, false, "Reset simulation", "Reset simulation", KEY_R};
    button_t resetCamButton = (button_t){(Rectangle){330, 10, 150, 40}, false, "Reset camera", "Reset camera", KEY_C};
    button_t loadButton = (button_t){(Rectangle){10, 10, 150, 40}, false, "Load new file", "Load new file", KEY_L};
                                    // Rectangle = { x, y, width, height };
                                    

    // Radius and distance scales initialization
    textbox_t Radius = {false, "0.003", strlen("0.003"), 0.003f,
                        (Rectangle){ info2.x + 20, 300, 180, 35 },
                        "Radius scale correction :"};

    textbox_t Dist   = {false, "390.0", strlen("390.0"), 390.0f,
                        (Rectangle){ info2.x + 20, 380, 180, 35 },
                        "Distance scale correction :"};
    
    textbox_t Dt   = {false, "10000.0", strlen("10000.0"), 10000.0f,
                        (Rectangle){ info2.x + 20, 460, 180, 35 },
                        "dt :"};
    
    textbox_t Previous   = {false, "100.0", strlen("100.0"), 100.0f,
                    (Rectangle){ info2.x + 20, 540, 180, 35 },
                    "number of previous values :"};
    
    textbox_t VectorScale = {false, "0.1", strlen("0.1"), 0.1f,
                    (Rectangle){ info2.x + 20, 620, 180, 35 },
                    "Velocity vector scale :"};

    textbox_t AUScale = {false, "30", strlen("30"), 30.0f,
                    (Rectangle){ info2.x + 20, 700, 180, 35 },
                    "AU scale :"};
    
    float screenRadius;

    ScaleType scalerad = SCALE_SQRT; // Default scale
    ScaleType scaledist = SCALE_LINEAR; // Default scale

    // Check buttons initialization
    Rectangle radlinearBtn = { screenWidth-290, 50, 16, 16 };
    Rectangle radlogBtn    = { screenWidth-290, 80, 16, 16 };
    Rectangle radsqrtBtn   = { screenWidth-290, 110, 16, 16 };

    Rectangle distlinearBtn = { screenWidth-290, 170, 16, 16 };
    Rectangle distlogBtn    = { screenWidth-290, 200, 16, 16 };
    Rectangle distsqrtBtn   = { screenWidth-290, 230, 16, 16 };

    // Open the file and collect the informations about the planets 
    char exec_dir[1024];
    char current_file[2048];
    strncpy(exec_dir, dirname(argv[0]), sizeof(exec_dir));
    snprintf(current_file, sizeof(current_file), "%s/simulation_files/systeme.txt", exec_dir);
    FILE *f = fopen(current_file, "r");
    if (f == NULL) {
        printf("Erreur fichier\n");
        perror(current_file);
        return 1;
    }
    int nbe;
    body_t* tab = collecting_data(f,&nbe);

    textbox_t* bodyBoxes = NULL;
    int bodyBoxNbe = 0;
    InitBodyTextBoxes(&bodyBoxes, &bodyBoxNbe, tab, nbe);

    // Camera initialization
    Camera2D camera;
    int targetIndex;
    bool centered;
    CamInitialization(&camera, &targetIndex, &centered, tab);
    
    float maxDist=distancemax(tab,nbe);
    float AU=maxDist/AUScale.value;

    //-----------------------------------------------------------------------------
    //      Main game loop
    //-----------------------------------------------------------------------------
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update the info rectangles and buttons positions to match the window size
        info.x = GetScreenWidth()-600;
        info.height = GetScreenHeight();
        info2.x = GetScreenWidth()-300;
        info2.height =  GetScreenHeight();
        Radius.rec.x = info2.x + 20;
        Dist.rec.x = info2.x + 20;
        Dt.rec.x = info2.x + 20;
        Previous.rec.x = info2.x + 20;
        VectorScale.rec.x = info2.x + 20;
        AUScale.rec.x = info2.x + 20;
        radlinearBtn.x  = GetScreenWidth()-290;
        radlogBtn.x     = GetScreenWidth()-290;
        radsqrtBtn.x    = GetScreenWidth()-290;
        distlinearBtn.x = GetScreenWidth()-290;
        distlogBtn.x    = GetScreenWidth()-290;
        distsqrtBtn.x   = GetScreenWidth()-290;

        // Detect and handle the clicks on the buttons
        Vector2 mouse = GetMousePosition();

        HandleClickOnButton(mouse, &pauseButton);
        HandleClickOnButton(mouse, &loadButton);
        HandleClickOnButton(mouse, &trailsButton);
        HandleClickOnButton(mouse, &vectorsButton);
        HandleClickOnButton(mouse, &resetSimButton);
        HandleClickOnButton(mouse, &resetCamButton);
      
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if (CheckCollisionPointRec(mouse, radlinearBtn)) scalerad = SCALE_LINEAR;
            if (CheckCollisionPointRec(mouse, radlogBtn))    scalerad = SCALE_LOG;
            if (CheckCollisionPointRec(mouse, radsqrtBtn))   scalerad = SCALE_SQRT;
            if (CheckCollisionPointRec(mouse, distlinearBtn)) scaledist = SCALE_LINEAR;
            if (CheckCollisionPointRec(mouse, distlogBtn))    scaledist = SCALE_LOG;
            if (CheckCollisionPointRec(mouse, distsqrtBtn))   scaledist = SCALE_SQRT;
        }

        // Handle text inputs in the textboxes
        char *activeText = NULL;
        int *activeCount = NULL;

        HandleTextInput(mouse, &Radius, activeText, activeCount);
        HandleTextInput(mouse, &Dist, activeText, activeCount);
        HandleTextInput(mouse, &Dt, activeText, activeCount);
        HandleTextInput(mouse, &Previous, activeText, activeCount);
        HandleTextInput(mouse, &VectorScale, activeText, activeCount);
        HandleTextInput(mouse, &AUScale, activeText, activeCount);

        float dt=Dt.value;
        float previous=Previous.value;
        if (!(previous >= 2.0f)) {
            previous = 2.0f;
        }
        if (previous > MAX_PREVIOUS) {
            previous = MAX_PREVIOUS;
        }

        float vectorScale = VectorScale.value;

        float wheel = GetMouseWheelMove();

        // Handle the scroll
        if (CheckCollisionPointRec(mouse, info)) {
            scrollOffset -= wheel * 20.0f;  
        }

        UpdateBodyTextBoxes(mouse, &bodyBoxes, &bodyBoxNbe, tab, nbe, info, scrollOffset);

        // Handle the camera
        // Zoom
        if (CheckCollisionPointRec(mouse, display) && wheel != 0) {
            Vector2 mouseBeforeZoom = GetScreenToWorld2D(GetMousePosition(), camera);

            camera.zoom = expf(logf(camera.zoom) + (wheel*0.1f)); // Uses log scaling to provide consistent zoom speed
            if (camera.zoom < 0.05f) camera.zoom = 0.05f;  // Avoid negative zoom

            // Recalculate after the zoom
            Vector2 mouseAfterZoom = GetScreenToWorld2D(GetMousePosition(), camera);
            
            // Compensate
            camera.target.x += mouseBeforeZoom.x - mouseAfterZoom.x;
            camera.target.y += mouseBeforeZoom.y - mouseAfterZoom.y;
        }

        // Position
        if (IsKeyDown(KEY_RIGHT)) {camera.offset.x -= GetScreenWidth()/100; centered = false;}
        else if (IsKeyDown(KEY_LEFT)) {camera.offset.x += GetScreenWidth()/100; centered = false;}
        else if (IsKeyDown(KEY_UP)) {camera.offset.y += GetScreenHeight()/100; centered = false;}
        else if (IsKeyDown(KEY_DOWN)) {camera.offset.y -= GetScreenHeight()/100; centered = false;}
        

        for (int i = 0; i < nbe; i++) {
            Vector2 mouseOnPlanets = GetScreenToWorld2D(GetMousePosition(), camera);
            Vector2 center;
            virtual (&screenRadius,&center, scaledist, scalerad, Radius.value, Dist.value, tab[i].radius, tab[i].position, AU);

            if (CheckCollisionPointCircle(mouseOnPlanets, center, screenRadius+25.0) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                targetIndex = i;
                centered = true;
                camera.offset = (Vector2){(GetScreenWidth() - 600) / 2, GetScreenHeight() / 2};
            }
        }


        //-----------------------------------------------------------------------------
        //      Draw
        //-----------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);

            // Draw circles to the screen
            for (int i = 0; i < nbe; i++) {
                Vector2 screenPos;
                virtual (&screenRadius,&screenPos, scaledist, scalerad, Radius.value, Dist.value, tab[i].radius, tab[i].position, AU);
                if (strcmp(tab[i].label,"soleil")==0){screenRadius=screenRadius/1.7;}
                DrawCircleV(screenPos, screenRadius, tab[i].color);
                Vector2* previous_pos = tab[i].previous_positions;

                if (trailsButton.var) { // Trails drawing
                    int r, g, b, a;
                    r = tab[i].color.r;
                    g = tab[i].color.g;
                    b = tab[i].color.b;
                    a = tab[i].color.a;
                    int nb_previous = (int)previous;

                    if (nb_previous > MAX_PREVIOUS) {
                        nb_previous = MAX_PREVIOUS;
                    }
                    if (nb_previous < 2) {
                        nb_previous = 2;
                    }

                    for (int j = 0; j < nb_previous - 1; j++) {
                        Vector2 p1;
                        Vector2 p2;
                        float uselessRadius;

                        virtual(&uselessRadius, &p1, scaledist, scalerad,
                                Radius.value, Dist.value,
                                tab[i].radius, previous_pos[j], AU);

                        virtual(&uselessRadius, &p2, scaledist, scalerad,
                                Radius.value, Dist.value,
                                tab[i].radius, previous_pos[j + 1], AU);

                        unsigned char alpha = (unsigned char)(a * (nb_previous - j) / nb_previous);

                        DrawLineEx(p1, p2, 2.0f/camera.zoom, (Color){r, g, b, alpha});
                    }
                }

                if (vectorsButton.var) {
                    float uselessRadius;

                    Vector2 futurePosition = {
                        tab[i].position.x + tab[i].velocity.x * dt * vectorScale,
                        tab[i].position.y + tab[i].velocity.y * dt * vectorScale
                    };

                    Vector2 futureScreenPos;

                    virtual(&uselessRadius, &futureScreenPos,
                            scaledist, scalerad,
                            Radius.value, Dist.value,
                            tab[i].radius, futurePosition, AU);

                    Vector2 screenVelocity = {
                        futureScreenPos.x - screenPos.x,
                        futureScreenPos.y - screenPos.y
                    };

                    drawVelocity(screenPos, screenVelocity, tab[i].color, 1.0f, 10.0f/camera.zoom, 2.0f/camera.zoom);
                }
            }

            if (targetIndex < 0 || targetIndex >= nbe) {
                centered = false;
            }

            if (centered) { // Follow the body targeted by the camera
                Vector2 targetBody;
                virtual (&screenRadius,&targetBody, scaledist, scalerad, Radius.value, Dist.value, tab[targetIndex].radius, tab[targetIndex].position, AU);
                camera.target = targetBody ;
            }

            EndMode2D();

            // Draw the help text
            DrawText(TextFormat("Frame time: %f s", GetFrameTime()), 10, GetScreenHeight() - 60, 20, DARKGRAY);
            DrawText("Click on a body to follow", 10, GetScreenHeight() - 40, 20, DARKGRAY);
            DrawText("Use arrows to pan", 10, GetScreenHeight() - 20, 20, DARKGRAY);

            // Buttons drawing
            DrawButton(pauseButton);
            DrawButton(loadButton);
            DrawButton(trailsButton);
            DrawButton(vectorsButton);
            DrawButton(resetSimButton);
            DrawButton(resetCamButton);

            // State text drawing
            if (pauseButton.var)
                DrawText("Simulation paused", (GetScreenWidth() - 600) / 2 - 140, GetScreenHeight() - 60, 35, RED);

            // Info rectangle drawing
            DrawBodyEditableInfo(info, &scrollOffset, bodyBoxes, tab, nbe);

            // Info2 rectangle drawing
            DrawRectangleRec(info2, LIGHTGRAY);
            DrawRectangleLinesEx(info2, 1, DARKGRAY);

            // Check buttons drawing
            DrawText("Scale radius", info2.x + 20, 15, 20, BLACK);
            DrawCircleLines(radlinearBtn.x + 8, radlinearBtn.y + 8, 8, BLACK);
            DrawText("Linear", radlinearBtn.x + 30, radlinearBtn.y - 2, 20, BLACK);
            DrawCircleLines(radlogBtn.x + 8, radlogBtn.y + 8, 8, BLACK);
            DrawText("Log", radlogBtn.x + 30, radlogBtn.y - 2, 20, BLACK);
            DrawCircleLines(radsqrtBtn.x + 8, radsqrtBtn.y + 8, 8, BLACK);
            DrawText("Sqrt", radsqrtBtn.x + 30, radsqrtBtn.y - 2, 20, BLACK);
            if (scalerad == SCALE_LINEAR) DrawCircle(radlinearBtn.x + 8, radlinearBtn.y + 8, 5, BLACK);
            if (scalerad == SCALE_LOG)    DrawCircle(radlogBtn.x + 8, radlogBtn.y + 8, 5, BLACK);
            if (scalerad == SCALE_SQRT)   DrawCircle(radsqrtBtn.x + 8, radsqrtBtn.y + 8, 5, BLACK);

            DrawText("Scale distance", info2.x + 20, 135, 20, BLACK);
            DrawCircleLines(distlinearBtn.x + 8, distlinearBtn.y + 8, 8, BLACK);
            DrawText("Linear", distlinearBtn.x + 30, distlinearBtn.y - 2, 20, BLACK);
            DrawCircleLines(distlogBtn.x + 8, distlogBtn.y + 8, 8, BLACK);
            DrawText("Log", distlogBtn.x + 30, distlogBtn.y - 2, 20, BLACK);
            DrawCircleLines(distsqrtBtn.x + 8, distsqrtBtn.y + 8, 8, BLACK);
            DrawText("Sqrt", distsqrtBtn.x + 30, distsqrtBtn.y - 2, 20, BLACK);
            if (scaledist == SCALE_LINEAR) DrawCircle(distlinearBtn.x + 8, distlinearBtn.y + 8, 5, BLACK);
            if (scaledist == SCALE_LOG)    DrawCircle(distlogBtn.x + 8, distlogBtn.y + 8, 5, BLACK);
            if (scaledist == SCALE_SQRT)   DrawCircle(distsqrtBtn.x + 8, distsqrtBtn.y + 8, 5, BLACK);
            
            // Textbox drawing
            DrawTextBox(Radius);
            DrawTextBox(Dist);
            DrawTextBox(Dt);
            DrawTextBox(Previous);
            DrawTextBox(VectorScale);
            DrawTextBox(AUScale);

            DrawText("ENTER", info2.x + 20, 740, 18, DARKGRAY);

            DrawText(TextFormat("radius_scale = %.5f", Radius.value), info2.x + 20, 780, 18, BLACK);
            DrawText(TextFormat("dist_scale = %.2f", Dist.value), info2.x + 20, 810, 18, BLACK);
            DrawText(TextFormat("dt = %.2f", Dt.value), info2.x + 20, 840, 18, BLACK);
            DrawText(TextFormat("previous = %.0f", previous), info2.x + 20, 870, 18, BLACK);
            DrawText(TextFormat("vector_scale = %.3f", vectorScale), info2.x + 20, 900, 18, BLACK);

        EndDrawing();
        

        //-----------------------------------------------------------------------------
        //      Next position and velocity calculations
        //-----------------------------------------------------------------------------
        if (!pauseButton.var) {

            handle_collisions(&tab, &nbe);

            update_physics(tab, nbe, dt);

            handle_collisions(&tab, &nbe);
        }

        // Delete a body that is too far
        if (distancemax(tab, nbe) > 100*maxDist) {
            body_t* newtab = HandleFarObjects(tab,&nbe);
            free(tab);
            tab = newtab;
            InitBodyTextBoxes(&bodyBoxes, &bodyBoxNbe, tab, nbe);
        }

        //-----------------------------------------------------------------------------
        //      Handle reset and change of file
        //-----------------------------------------------------------------------------

        // New file loading
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

        // Reset the simulation to start position
        if (resetSimButton.var) {
            FILE *f = fopen(current_file, "r");
            if (f == NULL) {
                printf("Erreur fichier\n");
                return 1;
            }

            int old_nbe = nbe;

            body_t* newtab = collecting_data(f, &nbe);

            free_bodies(tab, old_nbe);
            tab = newtab;

            maxDist = distancemax(tab, nbe);
            AU = maxDist / AUScale.value;

            InitBodyTextBoxes(&bodyBoxes, &bodyBoxNbe, tab, nbe);
            CamInitialization(&camera, &targetIndex, &centered, tab);

            resetSimButton.var = false;
        }

        // Reset the camera
        if (resetCamButton.var) {
            CamInitialization(&camera, &targetIndex, &centered, tab);
            resetCamButton.var = false;
        }

        nb_iter++;
    }

    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window
    //--------------------------------------------------------------------------------------
    free(bodyBoxes);
    free_bodies(tab, nbe);
    return 0;
}