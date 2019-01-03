#pragma once
//----------------------------------------------------------
//
//
//   	PathFinder Plugin
//                  by Pamdex and by Fleynaro
//
//
//	   
//----------------------------------------------------------
#include "genPath.h"

#define XYToNode(X,Y) (((Y - 3000) * -1) * 6000) + (X + 3000)

#define PATH_TYPE_POINT 1
#define PATH_TYPE_LINE 2
#define PATH_TYPE_CIRCLE 3

struct mapPoint;
class Path : public genPath
{
public:
	Path();
	std::map<int, mapPoint*> *mapData;
	
	int id, type;
	float startX, startY, startZ;
	int beginX, beginY;
	float endX, endY, endZ;
	float cellSize;
	float WALL_UP, WALL_DOWN;
	unsigned long uID;
	mapPoint *finalPoint;

	int SIZE_X, SIZE_Y;
	void setPlaneSize(int x, int y);

	bool checkWall(mapPoint *parent, mapPoint *child);
	void setDefaultBeginRelativeCoord();
	void setBeginRelativeCoord(float x, float y, int width, int height);
	void createPath(mapPoint * point);
	bool isFinal(mapPoint * point);
	bool check(float x1, float y1, float z1, float x2, float y2, float z2, float &x, float &y, float &z);
	void openCeils(mapPoint *parent, std::vector <mapPoint*> *ceils);
	mapPoint * getMapPoint(int x, int y);

	void getLocalXY(float X, float Y, short int &x, short int &y) {
		x = this->beginX + (int)round((X - this->startX) / this->cellSize);
		y = this->beginY + (int)round((Y - this->startY) / this->cellSize);
	}

	void Find() override;
	void Destroy() override;

	~Path();
private:
	int interpolation = 0;
};

struct mapPoint
{
	short int x = 0;
	short int y = 0;
	float z = 0.0;
	int parent = 0;
	int G = 0, F = 0;
	bool
		recheck = false,
		closed = false;
	
	Path * path;
	
	mapPoint(int x, int y, Path *path) {
		this->x = x;
		this->y = y;
		this->path = path;
	}

	mapPoint(float x, float y, Path *path) {
		this->path = path;
		this->setLocalXY(x, y);
	}

	void open()
	{
		if (this->isOpen()) return;
		this->path->mapData->insert(std::make_pair(this->getNodeCode(), this));
	}
	void getGlobalXY(float &x, float &y) {
		x = this->path->startX + (this->x - this->path->beginX) * this->path->cellSize;
		y = this->path->startY + (this->y - this->path->beginY) * this->path->cellSize;
	}
	void getGlobalXYZ(float &x, float &y, float &z) {
		this->getGlobalXY(x, y);
		z = this->z;
	}
	void setLocalXY(float x, float y) {
		this->path->getLocalXY(x, y, this->x, this->y);
	}
	void setG(int g) {
		this->G = g;
		this->F = this->getF();
	}
	bool isValid() {
		return (0 <= x && x < this->path->SIZE_X && 0 <= y && y < this->path->SIZE_Y);
	}
	bool isClosed() {
		return this->closed;
	}
	bool isOpen() {
		return (this->path->mapData->count(this->getNodeCode()) != 0);
	}
	bool isBegining() {
		return (this->x == this->path->beginX && this->y == this->path->beginY);
	}
	mapPoint *getParent() {
		return this->path->mapData->find(this->parent)->second;
	}
	int getNodeCode()
	{
		return XYToNode(this->x, this->y);
	}
	int getF()
	{
		int H;
		if (this->path->type != PATH_TYPE_LINE) {
			H = abs(this->x - this->path->finalPoint->x) + abs(this->y - this->path->finalPoint->y);
		}
		return this->G + H;
	}
};