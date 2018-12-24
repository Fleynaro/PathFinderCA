


#include "genPath.h"




genPath::genPath(CA_API *api)
{
	this->pathData = new std::deque <pathPoint*>;
	this->api = api;
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