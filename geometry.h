#pragma once


#include <cmath>


class geometry
{
public:
	geometry() {};
	~geometry() {};
	static float angleToRelative(float angle);
	static float differRelAngles(float alpha, float beta);
	static float toDegrees(float angle);
	static float PI() {
		return (float)3.141592653;
	}
	template <typename T> static int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}
	static float sq(float val) {
		return val * val;
	}
};

class Point3D
{
public:
	Point3D() {};
	Point3D(float x, float y, float z, int world = 0) {
		this->setPos(x, y, z);
		this->world = world;
	}
	~Point3D() {};
	void setPos(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	void setInvalid()
	{
		setPos(1000000.0, 1000000.0, 1000000.0);
	}
	bool isValid()
	{
		if (this->x != 1000000.0 && this->y != 1000000.0 && this->z != 1000000.0) {
			return true;
		}
		else {
			return false;
		}
	}
	float getX() {
		return this->x;
	}
	float getY() {
		return this->y;
	}
	float getZ() {
		return this->z;
	}
	int getWorld() {
		return this->world;
	}
private:
	float x, y, z;
	int world = 0;
};


class Vector3D
{
public:
	Vector3D() {};
	Vector3D(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vector3D(Point3D *point1, Point3D *point2) {
		this->x = point2->getX() - point1->getX();
		this->y = point2->getY() - point1->getY();
		this->z = point2->getZ() - point1->getZ();
	}
	~Vector3D() {};
	float getX() {
		return this->x;
	}
	float getY() {
		return this->y;
	}
	float getZ() {
		return this->z;
	}
	float getLong() {
		return sqrt(this->getLong2());
	}
	float getLong2() {
		return (this->x * this->x + this->y * this->y + this->z * this->z);
	}
	float getLongXY() {
		return sqrt(this->getLong2XY());
	}
	float getLong2XY() {
		return (this->x * this->x + this->y * this->y);
	}
	float getAngleXY();
	float getAngleBetweenXY(Vector3D * vector);
private:
	float x, y, z;
};


class Line
{
public:
	Line(Point3D *point1, Point3D *point2) {
		this->point = *point1;
		this->vector = Vector3D(point1, point2);
	}
	Line(Point3D *point, Vector3D *vector) {
		this->point = *point;
		this->vector = *vector;
	}
	Vector3D getVector() {
		return this->vector;
	}
	Point3D getFirstPoint() {
		return this->point;
	}
	Point3D getSecondPoint() {
		return Point3D(
			this->getFirstPoint().getX() + this->getVector().getX(),
			this->getFirstPoint().getY() + this->getVector().getY(),
			this->getFirstPoint().getZ() + this->getVector().getZ()
		);
	}
	bool isPointOnLine2D(Point3D *point);
private:
	Point3D point;
	Vector3D vector;
};