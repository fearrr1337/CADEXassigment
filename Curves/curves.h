#pragma once

#include <iostream>
#include <memory>
#include <vector>

#define PI 3.14159

// Экспорт/импорт для DLL:
// при сборке самой DLL определён CURVES_EXPORTS -> dllexport,
// при сборке exe, который её использует -> dllimport
#ifdef CURVES_EXPORTS
#define CURVES_API __declspec(dllexport)
#else
#define CURVES_API __declspec(dllimport)
#endif

struct CURVES_API Point3D {
	double x;
	double y;
	double z;

	Point3D(double px, double py, double pz);

	void print();
};

class CURVES_API Curve {
public:
	virtual ~Curve() = default;

	virtual Point3D getPoint(double t) = 0;

	virtual Point3D getDerivative(double t) = 0;
};

class CURVES_API Circle : public Curve {
private:
	double radius;
public:
	Circle(double r);

	double getRadius();

	Point3D getPoint(double t) override;

	Point3D getDerivative(double t) override;
};

class CURVES_API Ellipse : public Curve {
private:
	double radiusX;
	double radiusY;
public:
	Ellipse(double rx, double ry);

	Point3D getPoint(double t) override;

	Point3D getDerivative(double t) override;
};

class CURVES_API Helix : public Curve {
private:
	double radius;
	double step;
public:
	Helix(double r, double s);

	Point3D getPoint(double t) override;

	Point3D getDerivative(double t) override;
};

// визуализация кривых в окне raylib
CURVES_API void renderCurves(const std::vector<std::shared_ptr<Curve>>& curves);
