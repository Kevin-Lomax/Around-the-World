#include "raylib.h"
#include "raymath.h"
#include <vector> // Para gerenciar as cores facilmente

// Variáveis globais para o efeito de glitch (mais fácil de controlar)
bool glitchActive = false;
float glitchTimer = 0.0f;
float glitchDuration = 0.1f; // Glitch dura 0.1 segundos

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT); // Antialiasing para bordas suaves
    InitWindow(1280, 720, "DAFT PUNK LIVE SHOW");
    InitAudioDevice();

    Camera3D camera = { (Vector3){ 5, 5, 5 }, (Vector3){ 0, 0, 0 }, (Vector3){ 0, 1, 0 }, 45, 0 };
    
    // -- FORMAS 3D (NOVAS: CILINDRO E TORUS) --
    std::vector<Model> models;
    models.push_back(LoadModelFromMesh(GenMeshCube(2.0f, 2.0f, 2.0f)));
    models.push_back(LoadModelFromMesh(GenMeshSphere(1.5f, 32, 32)));
    models.push_back(LoadModelFromMesh(GenMeshCylinder(1.0f, 3.0f, 32))); // Cilindro
    models.push_back(LoadModelFromMesh(GenMeshTorus(0.5f, 1.5f, 16, 32))); // Torus (Rosquinha)
    
    int currentModelIndex = 0; // Começa com o cubo

    // -- TEXTURA (APLICADA A TODAS AS FORMAS) --
    Texture2D texture = LoadTexture("texture.png");
    for (Model& m : models) {
        m.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    }

    // -- MÚSICA --
    Music music = LoadMusicStream("music.mp3");
    PlayMusicStream(music);

    // -- CORES VIBRANTES --
    std::vector<Color> vibrantColors = {
        (Color){ 0, 255, 255, 255 },  // Ciano
        (Color){ 255, 0, 255, 255 },  // Magenta
        (Color){ 255, 255, 0, 255 },  // Amarelo
        (Color){ 0, 255, 0, 255 },    // Verde Neon
        (Color){ 255, 100, 0, 255 },  // Laranja Elétrico
        (Color){ 100, 0, 255, 255 }   // Roxo
    };
    int currentColorIndex = 0;
    Color activeColor = vibrantColors[currentColorIndex];

    // -- VARIÁVEIS DE ANIMAÇÃO --
    float rotationSpeed = 0.2f;
    float currentScale = 1.0f;
    float targetScale = 1.0f;
    float scaleSpeed = 5.0f; // Velocidade da animação de escala

    // -- EFEITO DE PÓS-PROCESSAMENTO (GLITCH) --
    // Shader que distorce a tela (pode ser um shader simples de noise/deslocamento)
    // Para um glitch real, precisaríamos de um shader customizado. 
    // Por enquanto, faremos um "piscar" com a câmera para simular.
    Shader glitchShader = LoadShader(0, "glitch.fs"); // Você precisaria criar este arquivo!
                                                      // Se não tiver, o efeito será apenas um flash.

    SetTargetFPS(144);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime(); // Tempo entre frames para animações suaves

        UpdateMusicStream(music);
        UpdateCamera(&camera, CAMERA_ORBITAL);

        // -- LÓGICA DO TECLADO PARA EFEITOS --
        if (IsKeyPressed(KEY_SPACE)) {
            currentColorIndex = (currentColorIndex + 1) % vibrantColors.size();
            activeColor = vibrantColors[currentColorIndex];
            targetScale = 1.2f; // Objeto "salta" ao mudar de cor
        }
        if (IsKeyPressed(KEY_LEFT)) {
            currentModelIndex = (currentModelIndex - 1 + models.size()) % models.size();
            targetScale = 1.0f; // Reseta escala
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            currentModelIndex = (currentModelIndex + 1) % models.size();
            targetScale = 1.0f; // Reseta escala
        }
        if (IsKeyPressed(KEY_UP)) rotationSpeed += 0.1f;
        if (IsKeyPressed(KEY_DOWN)) rotationSpeed -= 0.1f;

        // Tecla G para ativar o efeito "Glitch" (ou qualquer outra tecla que você queira)
        if (IsKeyPressed(KEY_G)) {
            glitchActive = true;
            glitchTimer = 0.0f;
        }

        // -- ANIMAÇÃO DE ESCALA --
        currentScale = Lerp(currentScale, targetScale, scaleSpeed * deltaTime);
        if (fabs(currentScale - targetScale) < 0.01f) targetScale = 1.0f; // Volta ao normal

        // -- ROTAÇÃO DO MODELO --
        models[currentModelIndex].transform = MatrixRotateY(GetTime() * rotationSpeed);

        BeginDrawing();
            ClearBackground((Color){ 10, 10, 20, 255 });

            // Ativa o shader de glitch se o efeito estiver ativo
            if (glitchActive) {
                glitchTimer += deltaTime;
                if (glitchTimer < glitchDuration) {
                    // Simulação de glitch: um flash e deslocamento da câmera
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){255, 0, 0, 50}); // Flash vermelho
                    camera.target.x += GetRandomValue(-10, 10) * 0.01f; // Desloca a câmera um pouco
                    camera.target.y += GetRandomValue(-10, 10) * 0.01f;
                } else {
                    glitchActive = false;
                    // Reseta a câmera para a posição original após o glitch
                    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
                }
            }


            BeginMode3D(camera);
                DrawModel(models[currentModelIndex], (Vector3){ 0, 0, 0 }, currentScale, activeColor);
                DrawModelWires(models[currentModelIndex], (Vector3){ 0, 0, 0 }, currentScale, (Color){200, 200, 255, 50}); // Linhas transparentes
                DrawGrid(20, 1.0f);
            EndMode3D();

            // -- INTERFACE --
            DrawRectangle(10, 10, 450, 140, Fade(BLACK, 0.7f));
            DrawText("DAFT PUNK LIVE: C++ EDITION", 20, 20, 22, GOLD);
            DrawText("[ESPAÇO]: Mudar Cor / Pular", 20, 50, 18, RAYWHITE);
            DrawText("[ESQUERDA/DIREITA]: Mudar Forma", 20, 75, 18, RAYWHITE);
            DrawText("[CIMA/BAIXO]: Velocidade Rotacao", 20, 100, 18, RAYWHITE);
            DrawText("[G]: Efeito GLITCH!", 20, 125, 18, (Color){255, 0, 0, 255});
            
            DrawFPS(1150, 20);
        EndDrawing();
    }

    // -- LIMPEZA --
    UnloadMusicStream(music);
    CloseAudioDevice();
    UnloadTexture(texture);
    for (Model& m : models) {
        UnloadModel(m);
    }
    UnloadShader(glitchShader); // Descarrega o shader (se existir)
    CloseWindow();

    return 0;
}
