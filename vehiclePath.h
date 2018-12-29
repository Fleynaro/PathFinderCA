#pragma once
#include "path.h"
#include "RoadPath.h"

class vehiclePath : public combinedPath
{
public:
	vehiclePath();
	~vehiclePath();

	void setStart(Point3D *pos) {
		this->start = *pos;
	}
	void setStart(roadNode *node) {
		this->start.setInvalid();
		this->startNode = node;
	}
	void setStart(Point3D *pos, roadNode *node) {
		this->start = *pos;
		this->startNode = node;
	}

	void setFinal(Point3D *pos) {
		this->end = *pos;
	}
	void setFinal(roadNode *node) {
		this->end.setInvalid();
		this->finalNode = node;
	}
	void setFinal(Point3D *pos, roadNode *node) {
		this->end = *pos;
		this->finalNode = node;
	}

	void setPathFinding(int i, bool value) {
		this->pathEnabled[i] = value;
	}

	void Find() override;
private:
	void setStart_(Point3D *pos, float roadRadius = 8.0);
	void setStart_(Point3D *pos, roadNode *node);
	void setStart_(roadNode *node);

	void setFinal_(Point3D *pos, float roadRadius = 8.0);
	void setFinal_(Point3D *pos, roadNode *node);
	void setFinal_(roadNode *node);

	Path *firstPath = NULL;
	Path *lastPath = NULL;
	RoadPath *roadPath = NULL;

	bool pathEnabled[2] = {true, true};

	Point3D start;
	roadNode *startNode = NULL;
	Point3D end;
	roadNode *finalNode = NULL;
	float roadRadius_ = 8.0;
};