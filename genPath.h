#pragma once


#include "CA_API.h"
#include "main.h"
#include <algorithm>

extern CA_API *api;


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
	enum status { FOUND, NOT_FOUND, PROCESS };
	status status;

	genPath();
	std::deque <pathPoint*> *pathData;
	std::string callback;
	std::deque<cell> params;
	int PATH_SIZE;
	unsigned long uID;
	void genPath::SetWorld(int world);
	int genPath::GetWorld();
	~genPath();

	virtual void Find() = 0;
	virtual void Destroy() = 0;
private:
	int world;
};