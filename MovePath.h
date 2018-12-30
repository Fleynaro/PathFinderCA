#pragma once


#include "main.h"
#include "geometry.h"


struct movePathPoint : public Point3D
{
public:
	movePathPoint(Point3D *pos) {
		this->setPos(pos->getX(), pos->getY(), pos->getZ());
	}
	~movePathPoint() {}
};

class MovePath
{
public:
	MovePath() {};
	~MovePath() {};
	void addPoint(movePathPoint *point) {
		this->points.push_back(*point);
	}
	movePathPoint *getPoint(int id) {
		return &this->points.front() + id;
	}
	bool isPointValid(int id) {
		return id >= 0 && id < this->size();
	}
	int getPointByPos(Point3D *pos, int lastPoint);
	int size() {
		return points.size();
	}
private:
	std::vector <movePathPoint> points;
};

