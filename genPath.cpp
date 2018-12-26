


#include "genPath.h"




genPath::genPath()
{
	this->pathData = new std::deque <pathPoint*>;
	this->world = 0;
}


void genPath::SetWorld(int world)
{
	this->world = world;
}

int genPath::GetWorld()
{
	return this->world;
}

genPath::~genPath()
{
}