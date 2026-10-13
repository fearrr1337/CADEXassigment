#include "curves.h"

#include <cmath>
#include <stdexcept>

Point3D::Point3D(double px, double py, double pz) : x(px), y(py), z(pz) {}

void Point3D::print() {
	std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
}

Circle::Circle(double r) : radius(r) {
	if (radius < 0) throw std::invalid_argument("Радиус не может быть отрицательным");
}

double Circle::getRadius() {
	return radius;
}

Point3D Circle::getPoint(double t) {
	double x = radius * std::cos(t);
	double y = radius * std::sin(t);
	double z = 0;

	return { x, y, z };
}

Point3D Circle::getDerivative(double t) {
	double x = -radius * std::sin(t);
	double y = radius * std::cos(t);
	double z = 0;

	return { x, y, z };
}

Ellipse::Ellipse(double rx, double ry) : radiusX(rx), radiusY(ry) {
	if (radiusX < 0 || radiusY < 0) throw std::invalid_argument("Неверные радиусы (<0)");
}

Point3D Ellipse::getPoint(double t) {
	double x = radiusX * std::cos(t);
	double y = radiusY * std::sin(t);
	double z = 0;

	return { x, y, z };
}

Point3D Ellipse::getDerivative(double t) {
	double x = -radiusX * std::sin(t);
	double y = radiusY * std::cos(t);
	double z = 0;

	return { x, y, z };
}

Helix::Helix(double r, double s) : radius(r), step(s) {
	if (radius < 0) throw std::invalid_argument("Радиус спирали должен быть > 0");
}

Point3D Helix::getPoint(double t) {
	double x = radius * std::cos(t);
	double y = radius * std::sin(t);
	double z = t * step;

	return { x, y, z };
}

Point3D Helix::getDerivative(double t) {
	double x = -radius * std::sin(t);
	double y = radius * std::cos(t);
	double z = step;

	return { x, y, z };
}
