#define CRT_SECURE_NO_WARNINGS
#include <vector>
#include <random>
#include <algorithm>
#include <memory>
#include <cmath>
#include <iostream>

#include "curves.h"

int main() {

	setlocale(LC_ALL, "C");

	std::vector<std::shared_ptr<Curve>> curves;
	std::vector<std::shared_ptr<Circle>> circles;

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
			double rx = modelParam(gen);
			double ry = modelParam(gen);
			newCurve = std::make_shared<Ellipse>(rx, ry);
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

		newCurve->setRotation(angleParam(gen), angleParam(gen), angleParam(gen));

		curves.push_back(newCurve);
	}

	double t = PI / 4;

	std::cout << "Coordinates of all shapes:" << std::endl;
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
	std::cout << "\n\n\nRadiuses of Circle objects from the second array:\n";
	for (const auto& c : circles) {
		std::cout << c->getRadius() << std::endl;
		sum_radius += c->getRadius();
	}

	std::cout << "\n\n\nSum of all radiuses from the second array: " << sum_radius << std::endl;

	renderCurves(curves);

	return 0;
}