#pragma once
#include <iostream>
#include <memory>
#include <vector>

#define PI 3.14159

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

	void setCenter(double cx, double cy, double cz);
	void setRotation(double rx, double ry, double rz);


	double getRotX() const { return rotX; }
	double getRotY() const { return rotY; }
	double getRotZ() const { return rotZ; }

	virtual Point3D getPoint(double t) = 0;
	virtual Point3D getDerivative(double t) = 0;

protected:
	Point3D center = { 0, 0, 0 };
	double rotX = 0.0;
	double rotY = 0.0;
	double rotZ = 0.0;

	Point3D transformPoint(const Point3D& p) const;
	Point3D transformDir(const Point3D& d) const;
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

CURVES_API void renderCurves(const std::vector<std::shared_ptr<Curve>>& curves);