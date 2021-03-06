#pragma once


#include "CA_API.h"
#include "main.h"
#include "geometry.h"
#include <algorithm>


struct pathPoint
{
	float x = 0.0, y = 0.0, z = 0.0;
	pathPoint() {}
	pathPoint(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

class genPath
{
public:
	enum state { FOUND, NOT_FOUND, PROCESS, NOT_STARTED };
	genPath::state status = NOT_STARTED;

	genPath();
	std::deque <pathPoint*> *pathData;
	std::string callback;
	std::deque<cell> params;
	int PATH_SIZE;
	unsigned long uID;
	void genPath::SetWorld(int world);
	int genPath::GetWorld();
	float getGenDist() {
		return this->genDist;
	}
	void setGenDist(float dist) {
		this->genDist = dist;
	}
	~genPath();

	virtual void Find() = 0;
	virtual void Destroy() = 0;
private:
	int world;
	float genDist = 0.0;
};


class combinedPath : public genPath
{
public:
	combinedPath();
	~combinedPath();
	void Find() override;
	void Destroy() override;
	void addPath(genPath *path);
private:
	std::deque <genPath*> paths;
};