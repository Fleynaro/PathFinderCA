
#include "path.h"



Path::Path()
{
	this->mapData = new std::map <int, mapPoint*>;
	
	this->id = -1;
	this->setPlaneSize(1024, 1024);
	this->setDefaultBeginRelativeCoord();

	this->cellSize = 1.5;
	this->type = PATH_TYPE_POINT;
	this->WALL_UP = 1.0;
	this->WALL_DOWN = 7.0;
	this->PATH_SIZE = 2000;
	
	mapPoint *parent = new mapPoint(0, 0, this);
	mapPoint *child = new mapPoint(10, 10, this);
}

void Path::setPlaneSize(int x, int y)
{
	this->SIZE_X = x;
	this->SIZE_Y = y;
}

void Path::setDefaultBeginRelativeCoord()
{
	this->beginX = this->SIZE_X / 2;
	this->beginY = this->SIZE_Y / 2;
}

void Path::setBeginRelativeCoord(float x, float y, int width = 10, int height = 10)
{
	Path::setDefaultBeginRelativeCoord();
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

bool Path::check(float x1, float y1, float z1, float x2, float y2, float z2, float &x, float &y, float &z)
{
	bool result = api->CA_RayCastLine(x1, y1, z1, x2, y2, z2, x, y, z, this->GetWorld()) != 0;
	return result;
}

bool Path::checkWall(mapPoint *parent, mapPoint *child)
{
	float x1, y1, x2, y2, temp;
	parent->getGlobalXY(x1, y1);
	child->getGlobalXY(x2, y2);
	
	bool result = (this->check(x2, y2, parent->z + this->WALL_UP, x2, y2, parent->z - this->WALL_DOWN, temp, temp, child->z) && -this->WALL_DOWN <= (child->z - parent->z) && (child->z - parent->z) <= this->WALL_UP);
	if (!result) {
		return 0;
	}
	result = !this->check(x1, y1, parent->z + float(0.5), x2, y2, child->z + float(0.5), temp, temp, temp);
	return result;
}

void Path::Find()
{
	if (this->status != NOT_STARTED) return;
	this->status = PROCESS;

	this->finalPoint = new mapPoint(this->endX, this->endY, this);
	this->finalPoint->z = this->endZ;
	
	mapPoint *point = this->getMapPoint(this->beginX, this->beginY);
	point->z = this->startZ;
	point->open();

	//if not found use this point as the closest
	mapPoint *nearestPoint = NULL;

	int stepCount = 0;
  //	this->interpolation = 3;
	while ( !this->isFinal(point) )
	{
		point->closed = true;

		if (this->interpolation > 0 && stepCount % this->interpolation == 0) //interpolation for opmtimization
		{
			float
				x1, y1, z1,
				x2, y2, z2;
			point->getGlobalXYZ(x1, y1, z1);
			mapPoint *endPoint = NULL;
			if (this->check(x1, y1, z1 + (float)0.5, this->endX, this->endY, this->endZ + (float)0.5, x2, y2, z2) == 0) {
				endPoint = this->finalPoint;
				x2 = this->endX;
				y2 = this->endY;
				z2 = this->endZ;
			} else {
				float Long = Vector3D(&Point3D(x1, y1, z1), &Point3D(x2, y2, z2)).getLong2();
				if (Long >= this->cellSize*this->cellSize * (5*5))
				{
					short int X, Y;
					this->getLocalXY(x2, y2, X, Y);
					endPoint = this->getMapPoint(X, Y);
					z2 -= (float)0.5;
				}
			}
			if (endPoint != NULL)
			{
				int Y_long = endPoint->y - point->y,
					X_long = endPoint->x - point->x,
					Max_long = abs(abs(Y_long) > abs(X_long) ? Y_long : X_long);
				float y_ = (float)point->y, x_ = (float)point->x;
				mapPoint *oldPoint_ = point;

				for ( int i = 0; i < Max_long; i ++)
				{
					y_ += (float)Y_long / Max_long;
					x_ += (float)X_long / Max_long;

					mapPoint *point_ = this->getMapPoint((int)floor(x_), (int)floor(y_));
					if (!point_->isValid() || point_->isClosed()) break;
					if (!point_->isOpen()) {
						point_->getGlobalXY(x1, y1);
						if (this->check(x1, y1, oldPoint_->z + this->WALL_UP, x1, y1, oldPoint_->z - this->WALL_DOWN, point_->z, point_->z, point_->z) == 0) {
							break;
						}

						point_->open();
						point_->parent = oldPoint_->getNodeCode();
						point_->setG(oldPoint_->G + 1);
					}
					else {
						if (oldPoint_->G + 1 < point_->G) {
							point_->parent = oldPoint_->getNodeCode();
							point_->setG(oldPoint_->G + 1);
						}
					}
					oldPoint_ = point_;
				}

				point = oldPoint_;
			}
			if (point == this->finalPoint) {
				break;
			}
		}
		stepCount++;
		
		std::vector <mapPoint*> ceils;
		this->openCeils(point, &ceils);
		std::sort(ceils.begin(), ceils.end(), [](const mapPoint *a, const mapPoint *b) {
			return a->F < b->F;
		});

		bool found = false;
		for (auto it = ceils.begin(); it != ceils.end(); it++) {
			mapPoint *curPoint = *it;
			if (this->checkWall(point, curPoint)) {
				if (curPoint->recheck && curPoint->parent != 0) {
					if (!this->checkWall(curPoint->getParent(), curPoint)) {
						curPoint->parent = point->getNodeCode();
						curPoint->setG(point->G + 1);
					}

				}
				point = curPoint;
				found = true;
				break;
			}
		}

		if (!found) {
			if (point->isBegining()) {
				this->status = NOT_FOUND;
				break;
			}
			point = point->getParent();
		}

		if ( this->mapData->size() > (size_t)this->PATH_SIZE ) {
			this->status = NOT_FOUND;
			break;
		}

		if (this->minPossibleH != 0) {
			if (!point->closed && point->getH() <= this->minPossibleH ) {
				if (nearestPoint == NULL || point->F < nearestPoint->F) {
					nearestPoint = point;
				}
			}
		}
	}
	if (this->status == NOT_FOUND) {
		if (nearestPoint != NULL) {
			point = nearestPoint;
		} else {
			return;
		}
	}

	this->setGenDist(point->G * this->cellSize);
	this->createPath(point);
}

void Path::createPath(mapPoint *point)
{
	int i = 0;
	while (i < this->PATH_SIZE) {
		if (point->isBegining()) {
			this->status = FOUND;
			return;
		}
		
		pathPoint *pathNode = new pathPoint();
		point->getGlobalXY(pathNode->x, pathNode->y);
		pathNode->z = point->z;
		this->pathData->push_back(pathNode);

		point = point->getParent();
		i++;
	}
	this->status = NOT_FOUND;
}

bool Path::isFinal(mapPoint *point)
{
	if (point->x == this->SIZE_X - 1 || point->y == this->SIZE_Y - 1) {
		this->status = FOUND;
		return true;
	}
	switch (this->type)
	{
		case PATH_TYPE_POINT: {
			if ( point->x == this->finalPoint->x && point->y == this->finalPoint->y && abs(point->z - this->finalPoint->z) <= this->cellSize ) {
				return true;
			} else return false;
		}
		case PATH_TYPE_CIRCLE: {
			float x, y;
			point->getGlobalXY(x, y);
			if (pow(x - this->endX, 2) + pow(y - this->endY, 2) <= pow(this->endZ, 2)) {
				return true;
			} else return false;
		}
	}
	return false;
}

void Path::openCeils(mapPoint *parent, std::vector <mapPoint*> *ceils)
{
	for (int rX = -1; rX <= 1; rX ++) {
		for (int rY = -1; rY <= 1; rY ++) {
			if (rX == 0 && rY == 0) continue;
			mapPoint *point = this->getMapPoint(parent->x + rX, parent->y + rY);
			
			if (point->isValid() && !point->isClosed()) {
				int G = parent->G + 1;
				if ( !point->isOpen() || (G <= point->G && this->checkWall(point->getParent(), point)) ) {
					point->parent = parent->getNodeCode();
					point->setG(G);
					point->recheck = false;
				}
				else {
					if (G > point->G) {
						point->recheck = true;
					}
				}
				point->open();
				ceils->push_back(point);
			}
		}
	}
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
}