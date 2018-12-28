


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







combinedPath::combinedPath()
{
	
}

void combinedPath::Find()
{
	for (const auto path : this->paths) {
		path->Find();
		if (path->status != genPath::state::FOUND) {
			this->status = path->status;
			return;
		}
		while (!path->pathData->empty()) {
			this->pathData->push_back(path->pathData->back());
			path->pathData->pop_back();
		}
	}
	this->status = state::FOUND;
}

void combinedPath::Destroy()
{
	delete this;
}

combinedPath::~combinedPath()
{
	for (auto path : this->paths) {
		path->Destroy();
	}
}

void combinedPath::addPath(genPath * path)
{
	this->paths.push_back(path);
}

void combinedPath::removePath(genPath * path)
{
	this->paths.erase(std::remove(this->paths.begin(), this->paths.end(), path), this->paths.end());
}
