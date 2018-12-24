#pragma once


#include "CA_API.h"
#include "main.h"
#include <algorithm>




struct pathPoint
{
	float x, y, z;
};

class genPath
{
public:
	static enum status { FOUND, NOT_FOUND, PROCESS };
	status status;

	genPath(CA_API *api);
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
	CA_API *api;
};