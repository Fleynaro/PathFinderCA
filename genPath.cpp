


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
	while (!this->paths.empty()) {
		genPath *path = this->paths.front();
		//logprintf("combinedPath::Find(): path=%i", path->uID);

		path->Find();
		if (path->status != genPath::state::FOUND) {
			this->status = path->status;
			return;
		}
		while (!path->pathData->empty()) {
			this->pathData->push_front(path->pathData->back());
			path->pathData->pop_back();
		}
		this->setGenDist(this->getGenDist() + path->getGenDist());

		path->Destroy();
		this->paths.pop_front();
	}
	this->status = genPath::state::FOUND;
}

void combinedPath::Destroy()
{
	delete this;
}

combinedPath::~combinedPath()
{
	while (!this->paths.empty()) {
		this->paths.front()->Destroy();
		this->paths.pop_front();
	}
}

void combinedPath::addPath(genPath * path)
{
	//logprintf("combinedPath::addPath: path=%i", path->uID);
	this->paths.push_back(path);
}
