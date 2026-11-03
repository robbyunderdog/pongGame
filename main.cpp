#include "raylib.h"

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    // Only show raylib's warnings and errors, not its startup INFO messages.
    SetTraceLogLevel(LOG_WARNING);

    // Create the window and aim for 60 frames per second.
    InitWindow(screenWidth, screenHeight, "Pong");
    SetTargetFPS(60);

    // --- Game objects ---
    // Paddle size, shared by both players.
    const float paddleWidth = 10.0f;
    const float paddleHeight = 80.0f;
    const float paddleSpeed = 400.0f;   // pixels per second

    // A Rectangle is { x, y, width, height }, where (x, y) is its top-left corner.
    // Both paddles start vertically centered: (screen height - paddle height) / 2.
    Rectangle leftPaddle  = { 30.0f, (screenHeight - paddleHeight) / 2, paddleWidth, paddleHeight };
    Rectangle rightPaddle = { screenWidth - 30.0f - paddleWidth, (screenHeight - paddleHeight) / 2, paddleWidth, paddleHeight };

    // The ball is a circle: a center point (Vector2 is { x, y }) and a radius.
    Vector2 ballPosition = { screenWidth / 2.0f, screenHeight / 2.0f };
    const float ballRadius = 8.0f;

    // Velocity = how far the ball moves per second on each axis (pixels per second).
    // Positive x = moving right, positive y = moving down.
    Vector2 ballVelocity = { 300.0f, 200.0f };

    int leftScore = 0;
    int rightScore = 0;

    // A small helper (a "lambda") that puts the ball back in the middle at its starting speed.
    // direction: -1 sends it left, +1 sends it right.
    // [&] lets the lambda use and change the variables above (ballPosition, ballVelocity).
    auto resetBall = [&](int direction)
    {
        ballPosition = { screenWidth / 2.0f, screenHeight / 2.0f };
        ballVelocity.x = 300.0f * direction;
        ballVelocity.y = GetRandomValue(0, 1) == 0 ? -200.0f : 200.0f;   // randomly up or down
    };

    // The game loop: runs once per frame until you close the window (or press Esc).
    while (!WindowShouldClose())
    {
        // --- Update ---
        // Seconds since the last frame (about 0.0167 at 60 FPS).
        float dt = GetFrameTime();

        // Left player: W = up, S = down. Remember y grows downward, so "up" means subtracting.
        if (IsKeyDown(KEY_W)) leftPaddle.y -= paddleSpeed * dt;
        if (IsKeyDown(KEY_S)) leftPaddle.y += paddleSpeed * dt;

        // Right player: arrow keys.
        if (IsKeyDown(KEY_UP))   rightPaddle.y -= paddleSpeed * dt;
        if (IsKeyDown(KEY_DOWN)) rightPaddle.y += paddleSpeed * dt;

        // Keep both paddles on screen. The lowest a paddle's top edge may go is screenHeight - paddleHeight.
        if (leftPaddle.y < 0) leftPaddle.y = 0;
        if (leftPaddle.y > screenHeight - paddleHeight) leftPaddle.y = screenHeight - paddleHeight;
        if (rightPaddle.y < 0) rightPaddle.y = 0;
        if (rightPaddle.y > screenHeight - paddleHeight) rightPaddle.y = screenHeight - paddleHeight;

        // Move the ball by its velocity, scaled by frame time (same idea as the paddles).
        ballPosition.x += ballVelocity.x * dt;
        ballPosition.y += ballVelocity.y * dt;

        // Bounce off the top and bottom walls by flipping the vertical direction.
        // We compare the ball's edge (center +/- radius), not its center, to the wall.
        if (ballPosition.y - ballRadius < 0)
        {
            ballPosition.y = ballRadius;   // push it back inside so it can't get stuck in the wall
            ballVelocity.y = -ballVelocity.y;
        }
        if (ballPosition.y + ballRadius > screenHeight)
        {
            ballPosition.y = screenHeight - ballRadius;
            ballVelocity.y = -ballVelocity.y;
        }

        // Bounce off the paddles.
        // We only bounce if the ball is moving TOWARD that paddle; otherwise a ball that's
        // still overlapping after a bounce would flip again next frame and get stuck.
        if (ballVelocity.x < 0 && CheckCollisionCircleRec(ballPosition, ballRadius, leftPaddle))
        {
            ballPosition.x = leftPaddle.x + leftPaddle.width + ballRadius;   // move it just outside the paddle
            ballVelocity.x = -ballVelocity.x * 1.05f;                       // reverse and speed up 5%

            // Where on the paddle did it hit? -1 = top edge, 0 = middle, +1 = bottom edge.
            float hit = (ballPosition.y - (leftPaddle.y + paddleHeight / 2)) / (paddleHeight / 2);
            ballVelocity.y = hit * 300.0f;
        }
        if (ballVelocity.x > 0 && CheckCollisionCircleRec(ballPosition, ballRadius, rightPaddle))
        {
            ballPosition.x = rightPaddle.x - ballRadius;
            ballVelocity.x = -ballVelocity.x * 1.05f;

            float hit = (ballPosition.y - (rightPaddle.y + paddleHeight / 2)) / (paddleHeight / 2);
            ballVelocity.y = hit * 300.0f;
        }

        // Scoring: if the ball gets past a paddle, the other player gets a point.
        // The ball is then served toward the player who just lost the point.
        if (ballPosition.x < 0)
        {
            rightScore++;
            resetBall(-1);
        }
        if (ballPosition.x > screenWidth)
        {
            leftScore++;
            resetBall(+1);
        }

        // --- Draw ---
        BeginDrawing();
        ClearBackground(BLACK);

        // Dashed center line, just for looks.
        for (int y = 0; y < screenHeight; y += 30)
        {
            DrawRectangle(screenWidth / 2 - 1, y, 2, 15, GRAY);
        }

        // Scores, one on each side of the center line.
        // TextFormat works like printf: %i is replaced by the integer.
        DrawText(TextFormat("%i", leftScore), screenWidth / 4, 20, 40, WHITE);
        DrawText(TextFormat("%i", rightScore), 3 * screenWidth / 4, 20, 40, WHITE);

        DrawRectangleRec(leftPaddle, WHITE);
        DrawRectangleRec(rightPaddle, WHITE);
        DrawCircleV(ballPosition, ballRadius, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
