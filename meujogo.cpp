#include "raylib.h"
#include "raymath.h"

int main() {
    // Ativa Anti-aliasing pesado
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "C++ REALISM ENGINE 100%");

    Camera3D camera = { (Vector3){ 10, 10, 10 }, (Vector3){ 0, 0, 0 }, (Vector3){ 0, 1, 0 }, 45, 0 };
    Shader shader = LoadShader(0, "pbr.fs");
    
    int lpLoc = GetShaderLocation(shader, "lightPos");
    int vpLoc = GetShaderLocation(shader, "viewPos");

    // Luz de estúdio (Posicionada para criar reflexos agressivos)
    Vector3 lightPos = { 20.0f, 25.0f, 15.0f };
    
    // Malha com densidade de 1 milhão de triângulos para curvas perfeitas
    Model model = LoadModelFromMesh(GenMeshKnot(1.5f, 4.0f, 600, 300));
    model.materials[0].shader = shader;
    // Cor Azul Cobalto Profundo
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = (Color){ 10, 50, 150, 255 };

    SetTargetFPS(144);

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_ORBITAL);
        
        SetShaderValue(shader, lpLoc, &lightPos, SHADER_UNIFORM_VEC3);
        SetShaderValue(shader, vpLoc, &camera.position, SHADER_UNIFORM_VEC3);

        BeginDrawing();
            ClearBackground((Color){ 5, 5, 8, 255 }); // Espaço profundo
            
            BeginMode3D(camera);
                // Desenha o objeto com brilho metálico
                DrawModel(model, Vector3Zero(), 1.0f, WHITE);
                DrawGrid(20, 1.0f);
            EndMode3D();

            DrawText("MATERIAL: CHROME STEEL (PBR)", 20, 20, 20, CYAN);
            DrawFPS(1200, 20);
        EndDrawing();
    }
    
    UnloadModel(model);
    UnloadShader(shader);
    CloseWindow();
    return 0;
}
