#include "raylib.h"
#include <vector>

struct Pipe {
    float x;
    float topHeight;
    float gap;
    bool passed;
};

int main() {
    const int screenWidth = 450;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Flappy Bird C++");
    SetTargetFPS(60);

    Texture2D bgSprite   = LoadTexture("assets/background.png");
    Texture2D birdSprite = LoadTexture("assets/bird.png");
    Texture2D pipeSprite = LoadTexture("assets/pipe.png");

    float birdX = 100.0f;
    float birdY = screenHeight / 2.0f;
    float velocity = 0.0f;
    const float gravity = 1200.0f;
    const float jumpForce = -400.0f;

    float bgX = 0.0f;
    const float bgSpeed = 60.0f;
    float bgScale = (float)screenHeight / bgSprite.height;

    std::vector<Pipe> pipes;
    float pipeSpawnTimer = 0.0f;
    const float pipeSpawnRate = 1.5f;
    const float pipeSpeed = 200.0f;

    bool gameOver = false;
    int score = 0;
    int highScore = 0;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        if (!gameOver) {
            if (IsKeyPressed(KEY_SPACE) || GetGestureDetected() == GESTURE_TAP) {
                velocity = jumpForce;
            }

            velocity += gravity * deltaTime;
            birdY += velocity * deltaTime;

            bgX -= bgSpeed * deltaTime;
            if (bgX <= -bgSprite.width * bgScale) {
                bgX += bgSprite.width * bgScale;
            }

            pipeSpawnTimer += deltaTime;
            if (pipeSpawnTimer >= pipeSpawnRate) {
                pipeSpawnTimer = 0.0f;
                float randomTopHeight = (float)GetRandomValue(100, 400);
                pipes.push_back({ (float)screenWidth, randomTopHeight, 180.0f, false });
            }

            for (size_t i = 0; i < pipes.size(); i++) {
                pipes[i].x -= pipeSpeed * deltaTime;

                Rectangle birdRect = { birdX, birdY, (float)birdSprite.width, (float)birdSprite.height };
                Rectangle topPipeRect = { pipes[i].x, 0, (float)pipeSprite.width, pipes[i].topHeight };
                
                float bottomPipeY = pipes[i].topHeight + pipes[i].gap;
                Rectangle bottomPipeRect = { pipes[i].x, bottomPipeY, (float)pipeSprite.width, (float)screenHeight - bottomPipeY };

                if (CheckCollisionRecs(birdRect, topPipeRect) || CheckCollisionRecs(birdRect, bottomPipeRect)) {
                    gameOver = true;
                }

                if (!pipes[i].passed && pipes[i].x < birdX) {
                    pipes[i].passed = true;
                    score++;
                    if (score > highScore) {
                        highScore = score;
                    }
                }
            }

            if (birdY > screenHeight || birdY < 0) {
                gameOver = true;
            }
        } else {
            if (IsKeyPressed(KEY_SPACE) || GetGestureDetected() == GESTURE_TAP) {
                birdY = screenHeight / 2.0f;
                velocity = 0.0f;
                pipes.clear();
                score = 0;
                gameOver = false;
            }
        }

        // Cálculo da rotação (inclinação) do pássaro com base na velocidade
        float rotation = velocity * 0.08f;
        if (rotation < -30.0f) rotation = -30.0f;
        if (rotation > 70.0f) rotation = 70.0f;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Desenhar Fundo
        DrawTextureEx(bgSprite, (Vector2){ bgX, 0 }, 0.0f, bgScale, WHITE);
        DrawTextureEx(bgSprite, (Vector2){ bgX + (bgSprite.width * bgScale), 0 }, 0.0f, bgScale, WHITE);

        // Desenhar Canos
        for (const auto& pipe : pipes) {
            Rectangle sourceRectTop = { 0, 0, (float)pipeSprite.width, -pipe.topHeight };
            Rectangle destRectTop = { pipe.x, 0, (float)pipeSprite.width, pipe.topHeight };
            DrawTexturePro(pipeSprite, sourceRectTop, destRectTop, {0,0}, 0.0f, WHITE);

            float bottomY = pipe.topHeight + pipe.gap;
            float bottomHeight = screenHeight - bottomY;
            Rectangle sourceRectBottom = { 0, 0, (float)pipeSprite.width, bottomHeight };
            Rectangle destRectBottom = { pipe.x, bottomY, (float)pipeSprite.width, bottomHeight };
            DrawTexturePro(pipeSprite, sourceRectBottom, destRectBottom, {0,0}, 0.0f, WHITE);
        }

        // Desenhar Pássaro com Rotação em torno do centro
        Rectangle birdSource = { 0.0f, 0.0f, (float)birdSprite.width, (float)birdSprite.height };
        Rectangle birdDest = { birdX + birdSprite.width / 2.0f, birdY + birdSprite.height / 2.0f, (float)birdSprite.width, (float)birdSprite.height };
        Vector2 birdOrigin = { (float)birdSprite.width / 2.0f, (float)birdSprite.height / 2.0f };
        DrawTexturePro(birdSprite, birdSource, birdDest, birdOrigin, rotation, WHITE);

        // Pontuação Atual e Recorde
        DrawText(TextFormat("Score: %d", score), 20, 20, 30, WHITE);
        DrawText(TextFormat("Best: %d", highScore), 20, 55, 25, YELLOW);

        if (gameOver) {
            DrawText("GAME OVER", screenWidth / 2 - 100, screenHeight / 2 - 40, 35, RED);
            DrawText(TextFormat("Recorde: %d", highScore), screenWidth / 2 - 60, screenHeight / 2 + 10, 22, GOLD);
            DrawText("Toque para Reiniciar", screenWidth / 2 - 110, screenHeight / 2 + 50, 20, DARKGRAY);
        }

        EndDrawing();
    }

    UnloadTexture(birdSprite);
    UnloadTexture(pipeSprite);
    UnloadTexture(bgSprite);

    CloseWindow();
    return 0;
}
