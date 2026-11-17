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

struct InputField {
	Rectangle bounds;
	std::string text;
	std::string label;
	bool active;
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

	std::vector<std::shared_ptr<Curve>> myCurves = curves;
	std::vector<bool> showFigure(myCurves.size(), true);

	// 0 - закрыто, 1 - Меню Создания (C), 2 - Меню Видимости (G)
	int activeMenu = 0;
	int addingType = -1;
	std::vector<InputField> addInputs;

	while (!WindowShouldClose()) {
		// --- ПЕРЕКЛЮЧЕНИЕ МЕНЮ ---
		// (Блокируем открытие/закрытие меню, если мы сейчас вводим параметры)
		if (addingType == -1) {
			if (IsKeyPressed(KEY_C)) {
				if (activeMenu == 1) { activeMenu = 0; DisableCursor(); }
				else { activeMenu = 1; EnableCursor(); }
			}
			if (IsKeyPressed(KEY_G)) {
				if (activeMenu == 2) { activeMenu = 0; DisableCursor(); }
				else { activeMenu = 2; EnableCursor(); }
			}
		}

		Vector2 mousePoint = GetMousePosition();
		bool mouseClicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

		// --- ЛОГИКА МЕНЮ СОЗДАНИЯ И УДАЛЕНИЯ (Кнопка C) ---
		if (activeMenu == 1) {
			if (addingType == -1) {
				// Удаление фигур
				for (size_t i = 0; i < myCurves.size(); i++) {
					if (30 + i * 30 > 400) break;
					Rectangle xBtn = { 240, 30.0f + i * 30.0f, 20, 20 };
					if (mouseClicked && CheckCollisionPointRec(mousePoint, xBtn)) {
						myCurves.erase(myCurves.begin() + i);
						showFigure.erase(showFigure.begin() + i);
						break;
					}
				}

				// Кнопки добавления
				Rectangle btnHelix = { 20, 430, 250, 40 };
				Rectangle btnEllipse = { 20, 480, 250, 40 };
				Rectangle btnCircle = { 20, 530, 250, 40 };

				if (mouseClicked) {
					if (CheckCollisionPointRec(mousePoint, btnHelix)) addingType = 0;
					else if (CheckCollisionPointRec(mousePoint, btnEllipse)) addingType = 1;
					else if (CheckCollisionPointRec(mousePoint, btnCircle)) addingType = 2;

					if (addingType != -1) {
						addInputs.clear();
						std::vector<std::string> labels;
						if (addingType == 0) labels = { "R", "Step", "CX", "CY", "CZ", "RotX", "RotY", "RotZ" };
						else if (addingType == 1) labels = { "RadX", "RadY", "CX", "CY", "CZ", "RotX", "RotY", "RotZ" };
						else if (addingType == 2) labels = { "R", "CX", "CY", "CZ", "RotX", "RotY", "RotZ" };

						float startX = 300, startY = 150;
						for (size_t i = 0; i < labels.size(); i++) {
							InputField f;
							f.label = labels[i];
							f.text = "";
							f.active = (i == 0);
							int row = i / 5;
							int col = i % 5;
							f.bounds = { startX + 20 + col * 80, startY + 80 + row * 60, 70, 30 };
							addInputs.push_back(f);
						}
					}
				}
			}
			else {
				// Окно ввода параметров
				if (mouseClicked) {
					for (auto& field : addInputs) {
						field.active = CheckCollisionPointRec(mousePoint, field.bounds);
					}
				}

				if (IsKeyPressed(KEY_TAB)) {
					for (size_t i = 0; i < addInputs.size(); i++) {
						if (addInputs[i].active) {
							addInputs[i].active = false;
							addInputs[(i + 1) % addInputs.size()].active = true;
							break;
						}
					}
				}

				for (auto& field : addInputs) {
					if (field.active) {
						int key = GetCharPressed();
						while (key > 0) {
							if ((key >= '0' && key <= '9') || key == '.' || key == '-') {
								field.text += (char)key;
							}
							key = GetCharPressed();
						}
						if (IsKeyPressed(KEY_BACKSPACE) && !field.text.empty()) {
							field.text.pop_back();
						}
					}
				}

				if (IsKeyPressed(KEY_ENTER)) {
					std::vector<float> vals;
					for (auto& f : addInputs) {
						float val = 0;
						std::stringstream ss(f.text.empty() ? "0" : f.text);
						ss >> val;
						vals.push_back(val);
					}

					std::shared_ptr<Curve> newCurve;
					float cx = 0, cy = 0, cz = 0, rx = 0, ry = 0, rz = 0;

					try {
						if (addingType == 0) {
							newCurve = std::make_shared<Helix>(vals[0] <= 0 ? 1 : vals[0], vals[1]);
							cx = vals[2]; cy = vals[3]; cz = vals[4]; rx = vals[5]; ry = vals[6]; rz = vals[7];
						}
						else if (addingType == 1) {
							newCurve = std::make_shared<Ellipse>(vals[0] <= 0 ? 1 : vals[0], vals[1] <= 0 ? 1 : vals[1]);
							cx = vals[2]; cy = vals[3]; cz = vals[4]; rx = vals[5]; ry = vals[6]; rz = vals[7];
						}
						else if (addingType == 2) {
							newCurve = std::make_shared<Circle>(vals[0] <= 0 ? 1 : vals[0]);
							cx = vals[1]; cy = vals[2]; cz = vals[3]; rx = vals[4]; ry = vals[5]; rz = vals[6];
						}

						if (newCurve) {
							newCurve->setCenter(cx, cy, cz);
							newCurve->setRotation(rx * PI / 180.0, ry * PI / 180.0, rz * PI / 180.0);
							myCurves.push_back(newCurve);
							showFigure.push_back(true);
						}
					}
					catch (...) {}
					addingType = -1;
				}

				if (IsKeyPressed(KEY_ESCAPE)) {
					addingType = -1;
				}
			}
		}
		// --- ЛОГИКА МЕНЮ ВИДИМОСТИ (Кнопка G) ---
		else if (activeMenu == 2) {
			for (size_t i = 0; i < myCurves.size(); i++) {
				if (60 + i * 30 > 400) break;
				Rectangle toggleBtn = { 190, 60.0f + i * 30.0f, 40, 20 };
				if (mouseClicked && CheckCollisionPointRec(mousePoint, toggleBtn)) {
					showFigure[i] = !showFigure[i];
				}
			}
		}
		// --- ЛОГИКА КАМЕРЫ ---
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
		}

		camera.position.x = camera.target.x + dist * cosf(angleX) * cosf(angleY);
		camera.position.y = camera.target.y + dist * cosf(angleX) * sinf(angleY);
		camera.position.z = camera.target.z + dist * sinf(angleX);

		// --- ОТРИСОВКА ---
		BeginDrawing();
		ClearBackground(COL_BG);
		BeginMode3D(camera);

		DrawLine3D({ 0,0,0 }, { 20,0,0 }, COL_AXIS_X);
		DrawLine3D({ 0,0,0 }, { 0,20,0 }, COL_AXIS_Y);
		DrawLine3D({ 0,0,0 }, { 0,0,20 }, COL_AXIS_Z);

		for (size_t i = 0; i < myCurves.size(); i++) {
			if (!showFigure[i]) continue; // Пропуск скрытых фигур

			const auto& curve = myCurves[i];
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

		if (activeMenu == 0) {
			int cx = GetScreenWidth() / 2;
			int cy = GetScreenHeight() / 2;
			DrawCircle(cx, cy, 3, WHITE);
			DrawCircleLines(cx, cy, 8, COL_CROSSHAIR);
		}

		DrawFPS(10, 10);
		DrawText("Mouse: rotate | Wheel: zoom | WASD: move | Space/Ctrl: up/down | Esc: exit", 10, 40, 18, LIGHTGRAY);
		DrawText("C: Open Manager Menu | G: Open Visibility Menu", 10, 60, 18, GREEN);

		// --- ОТРИСОВКА ИНТЕРФЕЙСА ---
		if (activeMenu != 0) {
			DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));
		}

		if (activeMenu == 1) {
			DrawRectangleRoundedLines({ 20, 20, 250, 400 }, 0.1f, 10, MAROON);
			for (size_t i = 0; i < myCurves.size(); i++) {
				if (30 + i * 30 > 380) { DrawText("...", 30, 30 + i * 30, 20, DARKGRAY); break; }

				std::string name = "Unknown";
				Color textColor = GRAY;
				if (std::dynamic_pointer_cast<Helix>(myCurves[i])) { name = "Helix"; textColor = SKYBLUE; }
				else if (std::dynamic_pointer_cast<Ellipse>(myCurves[i])) { name = "Ellipse"; textColor = SKYBLUE; }
				else if (std::dynamic_pointer_cast<Circle>(myCurves[i])) { name = "Circle"; textColor = SKYBLUE; }

				DrawText(name.c_str(), 35, 30 + i * 30, 20, textColor);
				DrawText("X", 240, 30 + i * 30, 20, SKYBLUE);
			}

			DrawRectangleRoundedLines({ 20, 430, 250, 40 }, 0.2f, 10, DARKBLUE);
			DrawText("Add Helix", 95, 440, 20, ORANGE);

			DrawRectangleRoundedLines({ 20, 480, 250, 40 }, 0.2f, 10, DARKBLUE);
			DrawText("Add Ellipse", 90, 490, 20, ORANGE);

			DrawRectangleRoundedLines({ 20, 530, 250, 40 }, 0.2f, 10, DARKBLUE);
			DrawText("Add Circle", 95, 540, 20, ORANGE);

			if (addingType != -1) {
				float startX = 300, startY = 150;
				Rectangle popup = { startX, startY, 440, 220 };
				DrawRectangleRec(popup, RAYWHITE);
				DrawRectangleRoundedLines(popup, 0.1f, 10, ORANGE);

				DrawText("Create figure", startX + 20, startY + 20, 20, ORANGE);
				DrawText("Enter parameters:", startX + 20, startY + 50, 16, DARKGRAY);

				for (auto& f : addInputs) {
					DrawRectangleLinesEx(f.bounds, 2, f.active ? RED : MAROON);
					DrawText(f.label.c_str(), f.bounds.x, f.bounds.y - 15, 14, ORANGE);
					DrawText(f.text.c_str(), f.bounds.x + 5, f.bounds.y + 5, 20, DARKGRAY);
					if (f.active && ((int)(GetTime() * 2) % 2 == 0)) {
						DrawText("_", f.bounds.x + 5 + MeasureText(f.text.c_str(), 20), f.bounds.y + 5, 20, RED);
					}
				}
				DrawText("TAB: next field | ENTER: save | ESC: cancel", startX + 20, startY + 190, 14, GRAY);
			}
		}
		else if (activeMenu == 2) {
			DrawRectangleRoundedLines({ 20, 20, 250, 400 }, 0.1f, 10, LIME);
			DrawText("Visibility Panel", 30, 25, 20, LIME);

			for (size_t i = 0; i < myCurves.size(); i++) {
				if (60 + i * 30 > 380) { DrawText("...", 30, 60 + i * 30, 20, DARKGRAY); break; }

				std::string name = "Unknown";
				if (std::dynamic_pointer_cast<Helix>(myCurves[i])) name = "Helix";
				else if (std::dynamic_pointer_cast<Ellipse>(myCurves[i])) name = "Ellipse";
				else if (std::dynamic_pointer_cast<Circle>(myCurves[i])) name = "Circle";

				// Пронумерованное название (например: [1] Helix)
				char buf[64];
				snprintf(buf, sizeof(buf), "[%d] %s", (int)(i + 1), name.c_str());

				Color textColor = showFigure[i] ? WHITE : GRAY;
				DrawText(buf, 30, 60 + i * 30, 18, textColor);

				// Кнопка ON/OFF
				Rectangle toggleBtn = { 190, 60.0f + i * 30.0f, 40, 20 };
				DrawRectangleRec(toggleBtn, showFigure[i] ? GREEN : DARKGRAY);
				DrawText(showFigure[i] ? "ON" : "OFF", 195, 62 + i * 30, 14, BLACK);
			}
		}

		EndDrawing();
	}

	CloseWindow();
}