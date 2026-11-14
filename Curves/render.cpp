#include "curves.h"
#include <cmath>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include "include/raylib.h"

const Color COL_BG = { 25, 25, 30, 255 };
const Color COL_AXIS_X = { 200, 60, 60, 255 };
const Color COL_AXIS_Y = { 60, 180, 60, 255 };
const Color COL_AXIS_Z = { 60, 120, 220, 255 };
const Color COL_CIRCLE = { 220, 80, 80, 255 };
const Color COL_ELLIPSE = { 80, 200, 120, 255 };
const Color COL_HELIX = { 80, 150, 240, 255 };
const Color COL_CROSSHAIR = { 255, 255, 255, 150 };

void renderCurves(const std::vector<std::shared_ptr<Curve>>& curves) {
	SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_MSAA_4X_HINT);
	InitWindow(0, 0, "3D Curves");
	SetTargetFPS(60);
	DisableCursor();

	Camera3D camera = { 0 };
	camera.up = { 0.0f, 0.0f, 1.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	camera.target = { 0.0f, 0.0f, 3.0f };

	float angleY = 0.8f;
	float angleX = 0.5f;
	float dist = 100.0f;
	float sens = 0.003f;

	bool showCircles = true;
	bool showEllipses = true;
	bool showHelixes = true;

	bool isInputMode = false;
	std::string inputText = "";

	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_C)) {
			isInputMode = !isInputMode;
			if (isInputMode) {
				EnableCursor();
			}
			else {
				DisableCursor();
			}
		}

		if (isInputMode) {
			int key = GetCharPressed();
			while (key > 0) {
				if ((key >= 32) && (key <= 125)) {
					inputText += (char)key;
				}
				key = GetCharPressed();
			}

			if (IsKeyPressed(KEY_BACKSPACE) && !inputText.empty()) {
				inputText.pop_back();
			}

			if (IsKeyPressed(KEY_ENTER)) {
				std::stringstream ss(inputText);
				double rx = 0, ry = 0, rz = 0;
				if (ss >> rx >> ry >> rz) {
					for (const auto& curve : curves) {
						curve->setRotation(rx * PI / 180.0, ry * PI / 180.0, rz * PI / 180.0);
					}
				}
				isInputMode = false;
				inputText = "";
				DisableCursor();
			}
		}
		else {
			Vector2 delta = GetMouseDelta();
			angleY -= delta.x * sens;
			angleX += delta.y * sens;
			if (angleX > 1.4f) angleX = 1.4f;
			if (angleX < -1.4f) angleX = -1.4f;

			dist -= GetMouseWheelMove() * 5.0f;
			if (dist < 20.0f) dist = 20.0f;
			if (dist > 400.0f) dist = 400.0f;

			float speed = 30.0f * GetFrameTime();

			Vector3 forward = {
				camera.target.x - camera.position.x,
				camera.target.y - camera.position.y,
				camera.target.z - camera.position.z
			};
			float flen = sqrtf(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
			if (flen > 0.001f) { forward.x /= flen; forward.y /= flen; forward.z /= flen; }

			Vector3 right = { forward.y, -forward.x, 0.0f };
			float rlen = sqrtf(right.x * right.x + right.y * right.y);
			if (rlen > 0.001f) { right.x /= rlen; right.y /= rlen; }

			if (IsKeyDown(KEY_W)) { camera.target.x += forward.x * speed; camera.target.y += forward.y * speed; }
			if (IsKeyDown(KEY_S)) { camera.target.x -= forward.x * speed; camera.target.y -= forward.y * speed; }
			if (IsKeyDown(KEY_A)) { camera.target.x -= right.x * speed;   camera.target.y -= right.y * speed; }
			if (IsKeyDown(KEY_D)) { camera.target.x += right.x * speed;   camera.target.y += right.y * speed; }
			if (IsKeyDown(KEY_SPACE))        camera.target.z += speed;
			if (IsKeyDown(KEY_LEFT_CONTROL)) camera.target.z -= speed;

			if (IsKeyPressed(KEY_G)) showCircles = !showCircles;
			if (IsKeyPressed(KEY_H)) showEllipses = !showEllipses;
			if (IsKeyPressed(KEY_J)) showHelixes = !showHelixes;
		}

		camera.position.x = camera.target.x + dist * cosf(angleX) * cosf(angleY);
		camera.position.y = camera.target.y + dist * cosf(angleX) * sinf(angleY);
		camera.position.z = camera.target.z + dist * sinf(angleX);

		BeginDrawing();
		ClearBackground(COL_BG);
		BeginMode3D(camera);

		DrawLine3D({ 0,0,0 }, { 20,0,0 }, COL_AXIS_X);
		DrawLine3D({ 0,0,0 }, { 0,20,0 }, COL_AXIS_Y);
		DrawLine3D({ 0,0,0 }, { 0,0,20 }, COL_AXIS_Z);

		for (const auto& curve : curves) {
			Color col = WHITE;
			if (std::dynamic_pointer_cast<Circle>(curve)) {
				if (!showCircles) continue;
				col = COL_CIRCLE;
			}
			else if (std::dynamic_pointer_cast<Ellipse>(curve)) {
				if (!showEllipses) continue;
				col = COL_ELLIPSE;
			}
			else if (std::dynamic_pointer_cast<Helix>(curve)) {
				if (!showHelixes) continue;
				col = COL_HELIX;
			}

			double tEnd = std::dynamic_pointer_cast<Helix>(curve) ? 6 * PI : 2 * PI;
			int steps = 200;
			Vector3 prev = { 0, 0, 0 };
			bool first = true;

			for (int s = 0; s <= steps; ++s) {
				double tt = tEnd * s / steps;
				Point3D p = curve->getPoint(tt);
				Vector3 cur = { (float)p.x, (float)p.y, (float)p.z };
				if (!first) DrawLine3D(prev, cur, col);
				prev = cur;
				first = false;
			}
		}

		EndMode3D();

		Vector2 sx = GetWorldToScreen({ 22,0,0 }, camera);
		Vector2 sy = GetWorldToScreen({ 0,22,0 }, camera);
		Vector2 sz = GetWorldToScreen({ 0,0,22 }, camera);
		DrawText("X", (int)sx.x, (int)sx.y, 24, COL_AXIS_X);
		DrawText("Y", (int)sy.x, (int)sy.y, 24, COL_AXIS_Y);
		DrawText("Z", (int)sz.x, (int)sz.y, 24, COL_AXIS_Z);

		if (!isInputMode) {
			int cx = GetScreenWidth() / 2;
			int cy = GetScreenHeight() / 2;
			DrawCircle(cx, cy, 3, WHITE);
			DrawCircleLines(cx, cy, 8, COL_CROSSHAIR);
		}

		DrawFPS(10, 10);
		DrawText("Mouse: rotate | Wheel: zoom | WASD: move | Space/Ctrl: up/down | Esc: exit", 10, 40, 18, LIGHTGRAY);
		DrawText("G: toggle Circles | H: toggle Ellipses | J: toggle Helixes", 10, 60, 18, LIGHTGRAY);
		DrawText("C: set rotation parameters (UI)", 10, 80, 18, GREEN);

		if (isInputMode) {
			DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
			DrawRectangle(GetScreenWidth() / 2 - 200, GetScreenHeight() / 2 - 50, 400, 100, RAYWHITE);
			DrawRectangleLines(GetScreenWidth() / 2 - 200, GetScreenHeight() / 2 - 50, 400, 100, DARKGRAY);

			DrawText("Enter rotation X Y Z (in degrees):", GetScreenWidth() / 2 - 180, GetScreenHeight() / 2 - 35, 20, DARKGRAY);

			std::string displayTxt = inputText;
			if ((int)(GetTime() * 2) % 2 == 0) displayTxt += "_";
			DrawText(displayTxt.c_str(), GetScreenWidth() / 2 - 180, GetScreenHeight() / 2, 30, MAROON);

			DrawText("Press ENTER to apply", GetScreenWidth() / 2 - 180, GetScreenHeight() / 2 + 60, 16, LIGHTGRAY);
		}

		EndDrawing();
	}

	CloseWindow();
}