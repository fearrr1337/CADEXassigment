#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <stdexcept>
#include <algorithm>


#define PI 3.14159


struct Point3D {
	double x;
	double y;
	double z;

	Point3D(double px, double py, double pz) : x(px), y(py), z(pz) {}

	void print() {
		std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
	}
};

class Curve {
public:
	virtual ~Curve() = default;

	virtual Point3D getPoint(double t) = 0;

	virtual Point3D getDerivative(double t) = 0;
};

class Circle : public Curve {
private:
	double radius;
public:
	Circle(double r) : radius(r) {
		if (radius < 0) throw std::invalid_argument("Радиус не может быть отрицательным");
	}

	double getRadius() {
		return radius;
	}


	Point3D getPoint(double t) {
		double x = radius * std::cos(t);
		double y = radius * std::sin(t);
		double z = 0;

		return { x,y,z };
	}

	Point3D getDerivative(double t) {
		double x = -radius * std::sin(t);
		double y = radius * std::cos(t);
		double z = 0;

		return { x,y,z };
	}
};

class Ellipse : public Curve {
private:
	double radiusX;
	double radiusY;
public:
	Ellipse(double rx, double ry) : radiusX(rx), radiusY(ry) {
		if (radiusX < 0 || radiusY < 0) throw std::invalid_argument("Неверные радиусы (<0)");
	}

	Point3D getPoint(double t) {
		double x = radiusX * std::cos(t);
		double y = radiusY * std::sin(t);
		double z = 0;

		return { x,y,z };
	}

	Point3D getDerivative(double t) {
		double x = -radiusX * std::sin(t);
		double y = radiusY * std::cos(t);
		double z = 0;

		return { x,y,z };
	}
};

class Helix : public Curve {
private:
	double radius;
	double step;
public:
	Helix(double r, double s) : radius(r), step(s) {
		if (radius < 0) throw std::invalid_argument("Радиус спирали должен быть > 0");
	}

	Point3D getPoint(double t) {
		double x = radius * std::cos(t);
		double y = radius * std::sin(t);
		double z = t * step;

		return { x,y,z };
	}

	Point3D getDerivative(double t) {
		double x = -radius * std::sin(t);
		double y = radius * std::cos(t);
		double z = step;

		return { x,y,z };
	}
};


int main() {
	setlocale(LC_ALL, "Russian");

	std::vector<Curve*> curves;
	std::vector<Circle*> circles;

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> model(0, 2);
	std::uniform_real_distribution<float> modelParam(1.0, 15.0);

	int totalObjects = 12;

	for (int i = 0; i < totalObjects; i++) {
		int type = model(gen);

		if (type == 0) {
			double r = modelParam(gen);
			curves.push_back(new Circle(r));
		}
		else if (type == 1) {
			double rx = modelParam(gen);
			double ry = modelParam(gen);
			curves.push_back(new Ellipse(rx, ry));
		}
		else if (type == 2) {
			double r = modelParam(gen);
			double s = modelParam(gen);
			curves.push_back(new Helix(r, s));
		}
	}

	double t = PI / 4;

	std::cout << "Координаты всех фигур" << std::endl;
	for (const auto& curve : curves) {
		Circle* circlePtr = dynamic_cast<Circle*>(curve);

		if (circlePtr != nullptr) {
			circles.push_back(circlePtr);
		}

		curve->getPoint(t).print();
		curve->getDerivative(t).print();
	}

	// сортировка массива circle по радиусу
	std::sort(circles.begin(), circles.end(), [](Circle* a, Circle* b) {
		return a->getRadius() < b->getRadius();
	});

	double sum_radius = 0;

	std::cout << "\n\n\nРадиусы Circle из 2 массива:\n";
	for (auto c : circles) {
		std::cout << c->getRadius() << std::endl;
		sum_radius += c->getRadius();
	}

	std::cout << "\n\n\nСумма всех радиусов из 2 массива: " << sum_radius << std::endl;

	
	for (auto curve : curves) {
		delete curve;
	}

	return 0;
}
