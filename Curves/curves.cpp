#include "curves.h"
#include <cmath>
#include <stdexcept>

Point3D::Point3D(double px, double py, double pz) : x(px), y(py), z(pz) {}

void Point3D::print() {
	std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
}


void Curve::setCenter(double cx, double cy, double cz) {
	center = { cx, cy, cz };
}

void Curve::setRotation(double rx, double ry, double rz) {
	rotX = rx;
	rotY = ry;
	rotZ = rz;
}

// Поворачиваем вокруг осей X, Y и Z по очереди.
static Point3D applyRotation(const Point3D& p, double rx, double ry, double rz) {
	double cx = std::cos(rx), sx = std::sin(rx);
	double cy = std::cos(ry), sy = std::sin(ry);
	double cz = std::cos(rz), sz = std::sin(rz);

	
	double x1 = p.x;
	double y1 = cx * p.y - sx * p.z;
	double z1 = sx * p.y + cx * p.z;

	
	double x2 = cy * x1 + sy * z1;
	double y2 = y1;
	double z2 = -sy * x1 + cy * z1;

	
	double x3 = cz * x2 - sz * y2;
	double y3 = sz * x2 + cz * y2;
	double z3 = z2;

	return { x3, y3, z3 };
}

Point3D Curve::transformPoint(const Point3D& p) const {
	Point3D r = applyRotation(p, rotX, rotY, rotZ);
	return { r.x + center.x, r.y + center.y, r.z + center.z };
}

// Перенос кривой не меняет её производную.
Point3D Curve::transformDir(const Point3D& d) const {
	return applyRotation(d, rotX, rotY, rotZ);
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

	return transformPoint({ x, y, z });
}

Point3D Circle::getDerivative(double t) {
	double x = -radius * std::sin(t);
	double y = radius * std::cos(t);
	double z = 0;

	return transformDir({ x, y, z });
}

Ellipse::Ellipse(double rx, double ry) : radiusX(rx), radiusY(ry) {
	if (radiusX < 0 || radiusY < 0) throw std::invalid_argument("Неверные радиусы (<0)");
}

Point3D Ellipse::getPoint(double t) {
	double x = radiusX * std::cos(t);
	double y = radiusY * std::sin(t);
	double z = 0;

	return transformPoint({ x, y, z });
}

Point3D Ellipse::getDerivative(double t) {
	double x = -radiusX * std::sin(t);
	double y = radiusY * std::cos(t);
	double z = 0;

	return transformDir({ x, y, z });
}

Helix::Helix(double r, double s) : radius(r), step(s) {
	if (radius < 0) throw std::invalid_argument("Радиус спирали должен быть > 0");
}

Point3D Helix::getPoint(double t) {
	double x = radius * std::cos(t);
	double y = radius * std::sin(t);
	double z = t * step;

	return transformPoint({ x, y, z });
}

Point3D Helix::getDerivative(double t) {
	double x = -radius * std::sin(t);
	double y = radius * std::cos(t);
	double z = step;

	return transformDir({ x, y, z });
}