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

    Texture2D bgSprite   = LoadTexture("background.png");
    Texture2D birdSprite = LoadTexture("bird.png");
    Texture2D pipeSprite = LoadTexture("pipe.png");

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

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextureEx(bgSprite, (Vector2){ bgX, 0 }, 0.0f, bgScale, WHITE);
        DrawTextureEx(bgSprite, (Vector2){ bgX + (bgSprite.width * bgScale), 0 }, 0.0f, bgScale, WHITE);

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

        DrawTexture(birdSprite, (int)birdX, (int)birdY, WHITE);

        DrawText(TextFormat("Score: %d", score), 20, 20, 30, WHITE);

        if (gameOver) {
            DrawText("GAME OVER", screenWidth / 2 - 100, screenHeight / 2 - 30, 35, RED);
            DrawText("Toque para Reiniciar", screenWidth / 2 - 110, screenHeight / 2 + 20, 20, DARKGRAY);
        }

        EndDrawing();
    }

    UnloadTexture(birdSprite);
    UnloadTexture(pipeSprite);
    UnloadTexture(bgSprite);

    CloseWindow();
    return 0;
}
