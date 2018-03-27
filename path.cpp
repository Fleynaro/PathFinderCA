//----------------------------------------------------------
//
//
//   	PathFinder Plugin
//                  by Pamdex
//
//
//	   
//----------------------------------------------------------
//                         INCLUDES
//----------------------------------------------------------
#include "main.h"
#include "mutex.h"
#include "path.h"
#include <algorithm>

//----------------------------------------------------------
//                         Class
//----------------------------------------------------------
Path::Path(Mutex *colAndreasQueue, Invoke *g_Invoke/*, Controller *pController*/)
{
	this->colAndreasQueue = colAndreasQueue;
	this->g_Invoke = g_Invoke;
	//this->pController = pController;

	this->mapData = new std::map <int, mapPoint*>;
	this->pathData = new std::deque <pathPoint*>;
	
	this->id = -1;
	this->SetPlaneSize(1024, 1024);
	this->SetDefaultBeginRelativeCoord();

	this->cellSize = 1.5;
	this->type = PATH_TYPE_POINT;
	this->WALL_UP = 1.0;
	this->WALL_DOWN = 7.0;
	this->PATH_SIZE = 2000;

	mapPoint *parent = new mapPoint(0, 0, this);
	mapPoint *child = new mapPoint(10, 10, this);
	//logprintf("wall = %i", this->CheckWall(parent, child));

	/*float x, y, z; int r;
	r = g_Invoke->CA_RayCastLine(-2043.0, -88.0, 36.0, -2043.0, -88.0, 0.0, &x, &y, &z);
	logprintf("x,y,z = %i, %f %f %f", r, x, y, z);*/
}

void Path::SetPlaneSize(int x, int y)
{
	this->SIZE_X = x;
	this->SIZE_Y = y;
}

void Path::SetDefaultBeginRelativeCoord()
{
	this->beginX = this->SIZE_X / 2;
	this->beginY = this->SIZE_Y / 2;
}

void Path::SetBeginRelativeCoord(float x, float y, int width = 10, int height = 10)
{
	Path::SetDefaultBeginRelativeCoord();
	if (x - this->startX > this->SIZE_X * this->cellSize) {
		this->beginX = width;
	}
	else if (this->startX - x > this->SIZE_X * this->cellSize) {
		this->beginX = this->SIZE_X - width;
	}
	if (y - this->startX > this->SIZE_Y * this->cellSize) {
		this->beginY = height;
	}
	else if (this->startX - y > this->SIZE_Y * this->cellSize) {
		this->beginY = this->SIZE_Y - height;
	}
}

bool Path::CheckWall(mapPoint *parent, mapPoint *child)
{
	float x1, y1, x2, y2, temp;
	parent->GetGlobalXY(x1, y1);
	child->GetGlobalXY(x2, y2);
	//logprintf("CA_RayCastLine (%f,%f | %f,%f)", x1, y1, x2, y2);

	/*x1 = (double)x1;
	x2 = (double)x2;
	y1 = (double)y1;
	y2 = (double)y2;
	temp = (double)temp;*/

	this->colAndreasQueue->Lock();
	bool result = (this->g_Invoke->CA_RayCastLine(x2, y2, parent->z + this->WALL_UP, x2, y2, parent->z - this->WALL_DOWN, &temp, &temp, &child->z) != 0 && -this->WALL_DOWN <= (child->z - parent->z) && (child->z - parent->z) <= this->WALL_UP && this->g_Invoke->CA_RayCastLine(x1, y1, parent->z + 0.5, x2, y2, child->z + 0.5, &temp, &temp, &temp) == 0);
	
	int a;
	a = this->g_Invoke->CA_RayCastLine(x2, y2, parent->z + this->WALL_UP, x2, y2, parent->z - this->WALL_DOWN, &temp, &temp, &temp);
	//logprintf("CA_RayCastLine a = %i,%f != 0 (%f,%f,%f %f,%f,%f)", a, temp, x2, y2, parent->z + this->WALL_UP, x2, y2, parent->z - this->WALL_DOWN);
	a = this->g_Invoke->CA_RayCastLine(x1, y1, parent->z + 0.5, x2, y2, child->z + 0.5, &temp, &temp, &temp);
	//logprintf("CA_RayCastLine b = %i,%f == 0 (%f,%f,%f %f,%f,%f)", a, temp, x1, y1, parent->z + 0.5, x2, y2, child->z + 0.5);
	this->colAndreasQueue->Unlock();
	return result;
}

void Path::Find()
{
	this->status = PATH_CALCULATING;
	//logprintf("this->cellSize = %f", this->cellSize);

	this->finalPoint = new mapPoint(this->endX, this->endY, this);
	//logprintf("this->finalPoint->x,y = %i,%i,%f %s", this->finalPoint->x, this->finalPoint->y, this->endZ, this->callback);
	//logprintf("this->start->x,y,z = %f,%f,%f", this->startX, this->startY, this->startZ);

	mapPoint *point = this->getMapPoint(this->beginX, this->beginY);
	point->z = this->startZ;
	point->open();

	//logprintf("point started %i,%i", point->x, point->y);
	while ( !IsFinal(point) )
	{
		point->closed = true;
		//logprintf("point(%i,%i,%f) to Final (%i,%i)", point->x, point->y, point->z, this->finalPoint->x, this->finalPoint->y);
		
		std::vector <mapPoint*> ceils;
		ceils = OpenCeils(point);
		std::sort(ceils.begin(), ceils.end(), [](const mapPoint *a, const mapPoint *b) {
			return a->F < b->F;
		});

		bool found = false;
		for (std::vector <mapPoint*>::iterator it = ceils.begin(); it != ceils.end(); it++) {
			mapPoint *curPoint = *it;
			//logprintf("-- OPEN CEILS point(%i,%i,%f)", curPoint->x, curPoint->y, curPoint->z);
			if (this->CheckWall(point, curPoint)) {
				//logprintf("---- pos Z = %f", curPoint->z);
				if (curPoint->recheck) {
					if (!this->CheckWall(curPoint->GetParent(), curPoint)) {
						curPoint->parent = point->GetNode();
						curPoint->SetG(point->G + 1);
					}

				}
				point = curPoint;
				found = true;
				break;
			}
		}

		if (!found) {
			if (point->IsBegining()) {
				//logprintf("Error: not found.");
				this->status = PATH_NOT_FOUND;
				return;
			}
			point = point->GetParent();
		}

		if ( this->mapData->size() > this->PATH_SIZE ) {
			//logprintf("Error: finding size more defined.");
			this->status = PATH_NOT_FOUND;
			return;
		}
	}
	//logprintf("yes! CreatePath");

	for (std::map<int, mapPoint*>::iterator i = this->mapData->begin(); i != this->mapData->end(); ++i)
	{
		//logprintf("%i) %i,%i,%f", i->first, i->second->x, i->second->y, i->second->z);
	}

	this->CreatePath(point);
}

void Path::CreatePath(mapPoint *point)
{
	int i = 0;
	while (i < this->PATH_SIZE) {
		if (point->IsBegining()) {
			this->status = PATH_FOUND;
			return;
		}
		//logprintf("point(%i,%i,%f  %i,%i,%f) to Start (%i,%i)", point->x, point->y, point->z, point->GetParent()->x, point->GetParent()->y, point->GetParent()->z, this->beginX, this->beginY);

		pathPoint *pathNode = new pathPoint();
		point->GetGlobalXY(pathNode->x, pathNode->y);
		pathNode->z = point->z;
		//logprintf("pathNode->x, pathNode->y, pathNode->z = %f,%f,%f", pathNode->x, pathNode->y, pathNode->z);
		this->pathData->push_back(pathNode);

		point = point->GetParent();
		i++;
	}
	this->status = PATH_NOT_FOUND;
}

bool Path::IsFinal(mapPoint *point)
{
	if (point->x == this->SIZE_X - 1 || point->y == this->SIZE_Y - 1) {
		this->status = PATH_FOUND;
		return true;
	}
	switch (this->type)
	{
		case PATH_TYPE_POINT: {
			if ( point->x == this->finalPoint->x && point->y == this->finalPoint->y ) {
				return true;
			} else return false;
		}
		case PATH_TYPE_CIRCLE: {
			float x, y;
			point->GetGlobalXY(x, y);
			if (pow(x - this->endX, 2) + pow(y - this->endY, 2) <= pow(this->endZ, 2)) {
				return true;
			} else return false;
		}
	}
	return false;
}

std::vector <mapPoint*> Path::OpenCeils(mapPoint *parent)
{
	std::vector <mapPoint*> ceils;

	for (int rX = -1; rX <= 1; rX ++) {
		for (int rY = -1; rY <= 1; rY ++) {
			if (rX == 0 && rY == 0) continue;
			mapPoint *point = this->getMapPoint(parent->x + rX, parent->y + rY);
			//logprintf("-- near point(%i,%i,%f) %i, %i", point->x, point->y, parent->z, point->IsValid(), point->IsClosed());

			if (point->IsValid() && !point->IsClosed()) {
				int G = parent->G + 1;
				//logprintf("---- G = %i(%i) F = %i", G, point->IsOpen(), point->GetF());
				if ( !point->IsOpen() || (G <= point->G && Path::CheckWall(point->GetParent(), point)) ) {
					point->parent = parent->GetNode();
					point->SetG(G);
					point->recheck = false;
					//logprintf("---- set parent %i,%i", parent->x, parent->y);
				}
				else {
					if (G > point->G) {
						point->recheck = true;
					}
				}
				point->open();
				ceils.push_back(point);
			}
		}
	}

	return ceils;
}

mapPoint* Path::getMapPoint(int x, int y)
{
	int point = XYToNode(x, y);
	if (this->mapData->count(point) != 0) {
		return this->mapData->find(point)->second;
	}
	else {
		return new mapPoint(x, y, this);
	}
}

void Path::Destroy()
{
	if (this->id == -1) {
		delete this;
		return;
	}
	//this->controller->RemovePath(this->id);
}

Path::~Path()
{
	for (std::map<int, mapPoint*>::iterator i = mapData->begin(); i != mapData->end(); ++i) {
		delete i->second;
	}
	for (std::deque <pathPoint*>::iterator i = pathData->begin(); i != pathData->end(); ++i) {
		delete *i;
	}
	
	delete mapData;
	delete pathData;
	delete finalPoint;
}