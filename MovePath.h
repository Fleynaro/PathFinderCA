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
	float getDistBetweenPoints(int p1, int p2);
	int size() {
		return points.size();
	}
	void setExtra(int index, cell value) {
		extra[index] = value;
	}
	cell getExtra(int index) {
		return extra[index];
	}
private:
	std::vector <movePathPoint> points;
	cell extra[MAX_MOVEPATH_EXTRA_VALUES];
};

