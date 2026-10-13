#define CRT_SECURE_NO_WARNINGS

#include <vector>
#include <random>
#include <algorithm>
#include <memory>

#include "curves.h"

int main() {
	setlocale(LC_ALL, "Russian");

	std::vector<std::shared_ptr<Curve>> curves;
	std::vector<std::shared_ptr<Circle>> circles;

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> model(0, 2);
	std::uniform_real_distribution<float> modelParam(1.0, 15.0);

	int totalObjects = 12;

	for (int i = 0; i < totalObjects; i++) {
		int type = model(gen);

		if (type == 0) {
			double r = modelParam(gen);
			curves.push_back(std::make_shared<Circle>(r));
		}
		else if (type == 1) {
			double rx = modelParam(gen);
			double ry = modelParam(gen);
			curves.push_back(std::make_shared<Ellipse>(rx, ry));
		}
		else if (type == 2) {
			double r = modelParam(gen);
			double s = modelParam(gen);
			curves.push_back(std::make_shared<Helix>(r, s));
		}
	}

	double t = PI / 4;

	std::cout << "Координаты всех фигур" << std::endl;
	for (const auto& curve : curves) {
		auto circlePtr = std::dynamic_pointer_cast<Circle>(curve);

		if (circlePtr != nullptr) {
			circles.push_back(circlePtr);
		}

		curve->getPoint(t).print();
		curve->getDerivative(t).print();
	}

	// сортировка массива circle по радиусу
	std::sort(circles.begin(), circles.end(), [](const std::shared_ptr<Circle>& a, const std::shared_ptr<Circle>& b) {
		return a->getRadius() < b->getRadius();
		});

	double sum_radius = 0;

	std::cout << "\n\n\nРадиусы Circle из 2 массива:\n";
	for (const auto& c : circles) {
		std::cout << c->getRadius() << std::endl;
		sum_radius += c->getRadius();
	}

	std::cout << "\n\n\nСумма всех радиусов из 2 массива: " << sum_radius << std::endl;


	renderCurves(curves);

	return 0;
}
