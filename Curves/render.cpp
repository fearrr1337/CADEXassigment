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

enum UIMode {
	UI_NONE,
	UI_LIST,  // Режим выбора фигуры из списка
	UI_EDIT   // Режим ввода наклона для выбранной фигуры
};

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

	// Массив для хранения видимости каждой из 12 фигур
	std::vector<bool> showFigure(curves.size(), true);

	UIMode currentUI = UI_NONE;
	int selectedIndex = 0;
	std::string inputText = "";

	while (!WindowShouldClose()) {
		// Открытие/закрытие меню по кнопке C (если мы не находимся в режиме ввода текста)
		if (IsKeyPressed(KEY_C) && currentUI != UI_EDIT) {
			if (currentUI == UI_NONE) {
				currentUI = UI_LIST;
				EnableCursor();
			}
			else {
				currentUI = UI_NONE;
				DisableCursor();
			}
		}

		if (currentUI == UI_LIST) {
			if (IsKeyPressed(KEY_UP) && selectedIndex > 0) selectedIndex--;
			if (IsKeyPressed(KEY_DOWN) && selectedIndex < (int)curves.size() - 1) selectedIndex++;

			if (IsKeyPressed(KEY_BACKSPACE)) {
				curves[selectedIndex]->setRotation(0.0, 0.0, 0.0);
			}

			if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ENTER)) {
				currentUI = UI_EDIT;
				inputText = "";
			}
		}
		else if (currentUI == UI_EDIT) {
			int key = GetCharPressed();
			while (key > 0) {
				if ((key >= '0' && key <= '9') || key == ' ' || key == '-' || key == '.') {
					inputText += (char)key;
				}
				key = GetCharPressed();
			}

			if (IsKeyPressed(KEY_BACKSPACE) && !inputText.empty()) {
				inputText.pop_back();
			}

			if (IsKeyPressed(KEY_ENTER)) {
				std::stringstream ss(inputText);
				float rx = 0, ry = 0, rz = 0;
				if (ss >> rx >> ry >> rz) {
					curves[selectedIndex]->setRotation(rx * PI / 180.0, ry * PI / 180.0, rz * PI / 180.0);
				}
				currentUI = UI_LIST;
				inputText = "";
			}

			if (IsKeyPressed(KEY_ESCAPE)) {
				currentUI = UI_LIST;
				inputText = "";
			}
		}
		else {
			// --- Управление камерой и видимостью фигур ---
			Vector2 delta = GetMouseDelta();
			angleY -= delta.x * sens;
			angleX += delta.y * sens;
			if (angleX > 1.4f) angleX = 1.4f;
			if (angleX < -1.4f) angleX = -1.4f;

			dist -= GetMouseWheelMove() * 5.0f;
			if (dist < 20.0f) dist = 20.0f;
			if (dist > 400.0f) dist = 400.0f;

			float speed = 30.0f * GetFrameTime();
			Vector3 forward = { camera.target.x - camera.position.x, camera.target.y - camera.position.y, camera.target.z - camera.position.z };
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

			// Индивидуальное скрытие фигур (цифры 1-0, -, =)
			if (curves.size() > 0 && IsKeyPressed(KEY_ONE))   showFigure[0] = !showFigure[0];
			if (curves.size() > 1 && IsKeyPressed(KEY_TWO))   showFigure[1] = !showFigure[1];
			if (curves.size() > 2 && IsKeyPressed(KEY_THREE)) showFigure[2] = !showFigure[2];
			if (curves.size() > 3 && IsKeyPressed(KEY_FOUR))  showFigure[3] = !showFigure[3];
			if (curves.size() > 4 && IsKeyPressed(KEY_FIVE))  showFigure[4] = !showFigure[4];
			if (curves.size() > 5 && IsKeyPressed(KEY_SIX))   showFigure[5] = !showFigure[5];
			if (curves.size() > 6 && IsKeyPressed(KEY_SEVEN)) showFigure[6] = !showFigure[6];
			if (curves.size() > 7 && IsKeyPressed(KEY_EIGHT)) showFigure[7] = !showFigure[7];
			if (curves.size() > 8 && IsKeyPressed(KEY_NINE))  showFigure[8] = !showFigure[8];
			if (curves.size() > 9 && IsKeyPressed(KEY_ZERO))  showFigure[9] = !showFigure[9];
			if (curves.size() > 10 && IsKeyPressed(KEY_MINUS)) showFigure[10] = !showFigure[10];
			if (curves.size() > 11 && IsKeyPressed(KEY_EQUAL)) showFigure[11] = !showFigure[11];
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

		// Отрисовка с проверкой индивидуальной видимости
		for (size_t i = 0; i < curves.size(); i++) {
			if (!showFigure[i]) continue; // Пропускаем фигуру, если она выключена

			const auto& curve = curves[i];
			Color col = WHITE;

			if (std::dynamic_pointer_cast<Circle>(curve)) col = COL_CIRCLE;
			else if (std::dynamic_pointer_cast<Ellipse>(curve)) col = COL_ELLIPSE;
			else if (std::dynamic_pointer_cast<Helix>(curve)) col = COL_HELIX;

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

		if (currentUI == UI_NONE) {
			int cx = GetScreenWidth() / 2;
			int cy = GetScreenHeight() / 2;
			DrawCircle(cx, cy, 3, WHITE);
			DrawCircleLines(cx, cy, 8, COL_CROSSHAIR);
		}

		DrawFPS(10, 10);
		DrawText("Mouse: rotate | Wheel: zoom | WASD: move | Space/Ctrl: up/down | Esc: exit", 10, 40, 18, LIGHTGRAY);
		DrawText("1-0, -, = : toggle individual figures visibility", 10, 60, 18, LIGHTGRAY);
		DrawText("C: manage individual figure rotations", 10, 80, 18, GREEN);

		if (currentUI != UI_NONE) {
			DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
		}

		if (currentUI == UI_LIST) {
			int panelW = 500;
			int panelH = 450;
			int panelX = GetScreenWidth() / 2 - panelW / 2;
			int panelY = GetScreenHeight() / 2 - panelH / 2;

			DrawRectangle(panelX, panelY, panelW, panelH, RAYWHITE);
			DrawRectangleLines(panelX, panelY, panelW, panelH, DARKGRAY);

			DrawText("Figures and their Rotations (X Y Z)", panelX + 20, panelY + 20, 20, DARKGRAY);

			int startY = panelY + 60;
			for (int i = 0; i < (int)curves.size() && i < 12; i++) {
				int itemY = startY + i * 26;
				Color textColor = DARKGRAY;

				if (i == selectedIndex) {
					DrawRectangle(panelX + 10, itemY - 2, panelW - 20, 26, LIGHTGRAY);
					textColor = BLACK;
				}

				std::string typeStr = "Unknown";
				if (std::dynamic_pointer_cast<Circle>(curves[i])) typeStr = "Circle";
				else if (std::dynamic_pointer_cast<Ellipse>(curves[i])) typeStr = "Ellipse";
				else if (std::dynamic_pointer_cast<Helix>(curves[i])) typeStr = "Helix";

				double rx = curves[i]->getRotX() * 180.0 / PI;
				double ry = curves[i]->getRotY() * 180.0 / PI;
				double rz = curves[i]->getRotZ() * 180.0 / PI;

				char buf[128];
				snprintf(buf, sizeof(buf), "[%d] %-8s: %.1f, %.1f, %.1f", i + 1, typeStr.c_str(), rx, ry, rz);
				DrawText(buf, panelX + 20, itemY, 18, textColor);
			}

			DrawText("UP/DOWN: Select Figure | ENTER / N: Edit Rotation", panelX + 10, panelY + panelH - 50, 14, GRAY);
			DrawText("BACKSPACE: Reset to 0 | C: Close", panelX + 10, panelY + panelH - 25, 14, GRAY);
		}
		else if (currentUI == UI_EDIT) {
			int panelW = 400;
			int panelH = 150;
			int panelX = GetScreenWidth() / 2 - panelW / 2;
			int panelY = GetScreenHeight() / 2 - panelH / 2;

			DrawRectangle(panelX, panelY, panelW, panelH, RAYWHITE);
			DrawRectangleLines(panelX, panelY, panelW, panelH, DARKGRAY);

			char titleBuf[64];
			snprintf(titleBuf, sizeof(titleBuf), "Set Rotation for Figure [%d]:", selectedIndex + 1);
			DrawText(titleBuf, panelX + 20, panelY + 20, 20, DARKGRAY);

			std::string displayTxt = inputText;
			if ((int)(GetTime() * 2) % 2 == 0) displayTxt += "_";
			DrawText(displayTxt.c_str(), panelX + 20, panelY + 60, 30, MAROON);

			DrawText("ENTER: Save | ESC: Cancel", panelX + 20, panelY + panelH - 30, 16, GRAY);
		}

		EndDrawing();
	}

	CloseWindow();
}