#define CRT_SECURE_NO_WARNINGS
#include <vector>
#include <random>
#include <algorithm>
#include <memory>
#include <cmath>
#include <iostream>

#include "curves.h"

int main() {
	setlocale(LC_ALL, "Russian");

	std::vector<std::shared_ptr<Curve>> curves;
	std::vector<std::shared_ptr<Circle>> circles;

	// --- ВВОД УГЛОВ НАКЛОНА ---
	double inputRotX = 0, inputRotY = 0, inputRotZ = 0;
	std::cout << "Введите угол наклона всех фигур по осям X, Y, Z в градусах (через пробел): ";
	std::cin >> inputRotX >> inputRotY >> inputRotZ;

	// Перевод градусов в радианы
	double rx = inputRotX * PI / 180.0;
	double ry = inputRotY * PI / 180.0;
	double rz = inputRotZ * PI / 180.0;
	// ---------------------------

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> model(0, 2);
	std::uniform_real_distribution<float> modelParam(2.0, 6.0);
	std::uniform_real_distribution<float> stepParam(0.2, 0.8);
	std::uniform_real_distribution<float> angleParam(0.0, 2.0 * PI);

	const double layoutRadius = 25.0;
	const int totalObjects = 12;

	for (int i = 0; i < totalObjects; i++) {
		int type = model(gen);

		std::shared_ptr<Curve> newCurve;

		if (type == 0) {
			double r = modelParam(gen);
			newCurve = std::make_shared<Circle>(r);
		}
		else if (type == 1) {
			double rx_param = modelParam(gen);
			double ry_param = modelParam(gen);
			newCurve = std::make_shared<Ellipse>(rx_param, ry_param);
		}
		else if (type == 2) {
			double r = modelParam(gen);
			double s = stepParam(gen);
			newCurve = std::make_shared<Helix>(r, s);
		}

		double layoutAngle = 2.0 * PI * i / totalObjects;
		double cz = (angleParam(gen) / (2.0 * PI) - 0.5) * 8.0;
		newCurve->setCenter(layoutRadius * std::cos(layoutAngle),
			layoutRadius * std::sin(layoutAngle),
			cz);

		// Устанавливаем наклон, введенный пользователем, вместо случайного
		newCurve->setRotation(rx, ry, rz);

		curves.push_back(newCurve);
	}

	double t = PI / 4;

	std::cout << "\nКоординаты всех фигур (t = PI/4):" << std::endl;
	for (const auto& curve : curves) {
		auto circlePtr = std::dynamic_pointer_cast<Circle>(curve);

		if (circlePtr != nullptr) {
			circles.push_back(circlePtr);
		}

		curve->getPoint(t).print();
		curve->getDerivative(t).print();
	}

	std::sort(circles.begin(), circles.end(), [](const std::shared_ptr<Circle>& a, const std::shared_ptr<Circle>& b) {
		return a->getRadius() < b->getRadius();
		});

	double sum_radius = 0;

	std::cout << "\nРадиусы Circle из 2 массива:\n";
	for (const auto& c : circles) {
		std::cout << c->getRadius() << std::endl;
		sum_radius += c->getRadius();
	}

	std::cout << "\nСумма всех радиусов из 2 массива: " << sum_radius << std::endl;

	// Запуск графического окна
	renderCurves(curves);

	return 0;
}