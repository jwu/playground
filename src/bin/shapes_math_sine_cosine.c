/*******************************************************************************************
 *
 *   raylib [shapes] example - math sine cosine
 *
 *   Example complexity rating: [★★☆☆] 2/4
 *
 *   Example originally created with raylib 6.0, last time updated with raylib 6.0
 *
 *   Example contributed by Jopestpe (@jopestpe) and reviewed by Ramon Santamaria (@raysan5)
 *
 *   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
 *   BSD-like license that allows static linking with closed source software
 *
 *   Copyright (c) 2025 Jopestpe (@jopestpe)
 *
 ********************************************************************************************/

#include "raylib.h"
#include <math.h>
#include <cglm/cglm.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h" // Required for GUI controls

// Wave points for sine/cosine visualization
#define WAVE_POINTS 36

static Vector2 Vec2ToVector2(vec2 v) {
  return (Vector2){ v[0], v[1] };
}

static float WrapDegrees(float angle) {
  angle = fmodf(angle, 360.0f);
  return (angle < 0.0f) ? (angle + 360.0f) : angle;
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "raylib [shapes] example - math sine cosine");

  Vector2 sinePoints[WAVE_POINTS];
  Vector2 cosPoints[WAVE_POINTS];
  vec2 center = { (screenWidth / 2.0f) - 30.0f, screenHeight / 2.0f };
  Rectangle start = { 20.0f, screenHeight - 120.f, 200.0f, 100.0f };
  float radius = 130.0f;
  float angle = 0.0f;
  bool pause = false;

  for (int i = 0; i < WAVE_POINTS; i++) {
    float t = i / (float)(WAVE_POINTS - 1);
    float currentAngle = glm_rad(glm_lerp(0.0f, 360.0f, t));
    vec2 waveOrigin = { start.x + glm_lerp(0.0f, start.width, t), start.y + start.height / 2.0f };
    vec2 sineOffset = { 0.0f, -sinf(currentAngle) * (start.height / 2.0f) };
    vec2 cosOffset = { 0.0f, -cosf(currentAngle) * (start.height / 2.0f) };
    vec2 sinePoint;
    vec2 cosPoint;

    glm_vec2_add(waveOrigin, sineOffset, sinePoint);
    glm_vec2_add(waveOrigin, cosOffset, cosPoint);

    sinePoints[i] = Vec2ToVector2(sinePoint);
    cosPoints[i] = Vec2ToVector2(cosPoint);
  }

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  // Detect window close button or ESC key
  while (!WindowShouldClose()) {
    // Update
    //----------------------------------------------------------------------------------
    float angleRad = glm_rad(angle);
    float cosRad = cosf(angleRad);
    float sinRad = sinf(angleRad);

    vec2 pointDir = { cosRad, -sinRad };
    vec2 pointOffset;
    vec2 radiusVec = { radius, radius };
    vec2 point;
    vec2 limitMin;
    vec2 limitMax;

    glm_vec2_scale(pointDir, radius, pointOffset);
    glm_vec2_add(center, pointOffset, point);
    glm_vec2_sub(center, radiusVec, limitMin);
    glm_vec2_add(center, radiusVec, limitMax);

    float complementary = 90.0f - angle;
    float supplementary = 180.0f - angle;
    float explementary = 360.0f - angle;

    float tangent = glm_clamp(tanf(angleRad), -10.0f, 10.0f);
    float cotangent = (fabsf(tangent) > 0.001f) ? glm_clamp(1.0f / tangent, -radius, radius) : 0.0f;
    vec2 tangentOffset = { radius, -tangent * radius };
    vec2 cotangentOffset = { cotangent * radius, -radius };
    vec2 tangentPoint;
    vec2 cotangentPoint;
    vec2 sineMarker = { start.x + glm_lerp(0.0f, start.width, angle / 360.0f),
                        start.y + ((-sinRad + 1.0f) * start.height / 2.0f) };
    vec2 cosMarker = { start.x + glm_lerp(0.0f, start.width, angle / 360.0f),
                       start.y + ((-cosRad + 1.0f) * start.height / 2.0f) };

    glm_vec2_add(center, tangentOffset, tangentPoint);
    glm_vec2_add(center, cotangentOffset, cotangentPoint);

    angle = WrapDegrees(angle + (!pause ? 1.0f : 0.0f));

    //----------------------------------------------------------------------------------
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Cotangent (orange)
    DrawLineEx(
      (Vector2){ center[0], limitMin[1] }, (Vector2){ cotangentPoint[0], limitMin[1] }, 2.0f, ORANGE
    );
    DrawLineDashed(Vec2ToVector2(center), Vec2ToVector2(cotangentPoint), 10, 4, ORANGE);

    // Side background
    DrawLine(580, 0, 580, GetScreenHeight(), (Color){ 218, 218, 218, 255 });
    DrawRectangle(580, 0, GetScreenWidth(), GetScreenHeight(), (Color){ 232, 232, 232, 255 });

    // Base circle and axes
    DrawCircleLinesV(Vec2ToVector2(center), radius, GRAY);
    DrawLineEx(
      (Vector2){ center[0], limitMin[1] }, (Vector2){ center[0], limitMax[1] }, 1.0f, GRAY
    );
    DrawLineEx(
      (Vector2){ limitMin[0], center[1] }, (Vector2){ limitMax[0], center[1] }, 1.0f, GRAY
    );

    // Wave graph axes
    DrawLineEx(
      (Vector2){ start.x, start.y }, (Vector2){ start.x, start.y + start.height }, 2.0f, GRAY
    );
    DrawLineEx(
      (Vector2){ start.x + start.width, start.y },
      (Vector2){ start.x + start.width, start.y + start.height }, 2.0f, GRAY
    );
    DrawLineEx(
      (Vector2){ start.x, start.y + start.height / 2 },
      (Vector2){ start.x + start.width, start.y + start.height / 2 }, 2.0f, GRAY
    );

    // Wave graph axis labels
    DrawText("1", (int)start.x - 8, (int)start.y, 6, GRAY);
    DrawText("0", (int)start.x - 8, (int)start.y + (int)start.height / 2 - 6, 6, GRAY);
    DrawText("-1", (int)start.x - 12, (int)start.y + (int)start.height - 8, 6, GRAY);
    DrawText("0", (int)start.x - 2, (int)start.y + (int)start.height + 4, 6, GRAY);
    DrawText(
      "360", (int)start.x + (int)start.width - 8, (int)start.y + (int)start.height + 4, 6, GRAY
    );

    // Sine (red - vertical)
    DrawLineEx((Vector2){ center[0], center[1] }, (Vector2){ center[0], point[1] }, 2.0f, RED);
    DrawLineDashed((Vector2){ point[0], center[1] }, Vec2ToVector2(point), 10, 4, RED);
    DrawText(TextFormat("Sine %.2f", sinRad), 640, 190, 6, RED);
    DrawCircleV(Vec2ToVector2(sineMarker), 4.0f, RED);
    DrawSplineLinear(sinePoints, WAVE_POINTS, 1.0f, RED);

    // Cosine (blue - horizontal)
    DrawLineEx((Vector2){ center[0], center[1] }, (Vector2){ point[0], center[1] }, 2.0f, BLUE);
    DrawLineDashed((Vector2){ center[0], point[1] }, Vec2ToVector2(point), 10, 4, BLUE);
    DrawText(TextFormat("Cosine %.2f", cosRad), 640, 210, 6, BLUE);
    DrawCircleV(Vec2ToVector2(cosMarker), 4.0f, BLUE);
    DrawSplineLinear(cosPoints, WAVE_POINTS, 1.0f, BLUE);

    // Tangent (purple)
    DrawLineEx(
      (Vector2){ limitMax[0], center[1] }, (Vector2){ limitMax[0], tangentPoint[1] }, 2.0f, PURPLE
    );
    DrawLineDashed(Vec2ToVector2(center), Vec2ToVector2(tangentPoint), 10, 4, PURPLE);
    DrawText(TextFormat("Tangent %.2f", tangent), 640, 230, 6, PURPLE);

    // Cotangent (orange)
    DrawText(TextFormat("Cotangent %.2f", cotangent), 640, 250, 6, ORANGE);

    // Complementary angle (beige)
    DrawCircleSectorLines(Vec2ToVector2(center), radius * 0.6f, -angle, -90.0f, 36, BEIGE);
    DrawText(TextFormat("Complementary  %0.f°", complementary), 640, 150, 6, BEIGE);

    // Supplementary angle (darkblue)
    DrawCircleSectorLines(Vec2ToVector2(center), radius * 0.5f, -angle, -180.0f, 36, DARKBLUE);
    DrawText(TextFormat("Supplementary  %0.f°", supplementary), 640, 130, 6, DARKBLUE);

    // Explementary angle (pink)
    DrawCircleSectorLines(Vec2ToVector2(center), radius * 0.4f, -angle, -360.0f, 36, PINK);
    DrawText(TextFormat("Explementary  %0.f°", explementary), 640, 170, 6, PINK);

    // Current angle - arc (lime), radius (black), endpoint (black)
    DrawCircleSectorLines(Vec2ToVector2(center), radius * 0.7f, -angle, 0.0f, 36, LIME);
    DrawLineEx(Vec2ToVector2(center), Vec2ToVector2(point), 2.0f, BLACK);
    DrawCircleV(Vec2ToVector2(point), 4.0f, BLACK);

    // Draw GUI controls
    //------------------------------------------------------------------------------
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(GRAY));
    GuiToggle((Rectangle){ 640, 70, 120, 20 }, TextFormat("Pause"), &pause);
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(LIME));
    GuiSliderBar(
      (Rectangle){ 640, 40, 120, 20 }, "Angle", TextFormat("%.0f°", angle), &angle, 0.0f, 360.0f
    );

    // Angle values panel
    GuiGroupBox((Rectangle){ 620, 110, 140, 170 }, "Angle Values");
    //------------------------------------------------------------------------------

    DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
