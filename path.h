//----------------------------------------------------------
//
//
//   	PathFinder Plugin
//                  by Pamdex
//
//
//	   
//----------------------------------------------------------
#include <Invoke/Invoke.h>

#define XYToNode(X,Y) (((Y - 3000) * -1) * 6000) + (X + 3000)
#define PATH_CALCULATING 1
#define PATH_FOUND 2
#define PATH_NOT_FOUND 3

#define PATH_TYPE_POINT 1
#define PATH_TYPE_LINE 2
#define PATH_TYPE_CIRCLE 3

struct pathPoint
{
	float x, y, z;
};
struct mapPoint;

class Path
{
public:
	Path(Mutex *colAndreasQueue, Invoke *g_Invoke/*, Controller *pController*/);
	std::deque <pathPoint*> *pathData;
	std::map<int, mapPoint*> *mapData;

	//Controller *pController;
	char callback[30];
	std::deque<cell> params;
	int id, type, status;
	float startX, startY, startZ;
	int beginX, beginY;
	float endX, endY, endZ;
	float cellSize;
	float WALL_UP, WALL_DOWN;
	int PATH_SIZE;
	mapPoint *finalPoint;

	int SIZE_X, SIZE_Y;
	void SetPlaneSize(int x, int y);

	bool CheckWall(mapPoint *parent, mapPoint *child);
	void SetDefaultBeginRelativeCoord();
	void SetBeginRelativeCoord(float x, float y, int width, int height);
	void Find();
	void CreatePath(mapPoint * point);
	bool IsFinal(mapPoint * point);
	std::vector <mapPoint*> OpenCeils(mapPoint * point);
	mapPoint * getMapPoint(int x, int y);

	void Destroy();

	~Path();
private:
	Mutex * colAndreasQueue;
	Invoke *g_Invoke;
};

struct mapPoint
{
	int x = 0;
	int y = 0;
	float z = 0.0;
	int parent = 0;
	int G = 0, F = 0;
	bool recheck = false, closed = false;
	
	Path * path;
	
	mapPoint(int x, int y, Path *path) {
		this->x = x;
		this->y = y;
		this->path = path;
	}

	mapPoint(float x, float y, Path *path) {
		this->path = path;
		this->SetLocalXY(x, y);
	}

	void open()
	{
		if (this->IsOpen()) return;
		this->path->mapData->insert(std::make_pair(this->GetNode(), this));
	}
	void GetGlobalXY(float &x, float &y) {
		//logprintf("GetGlobalXY (%f,%f | %f,%f)", this->path->startX);
		x = this->path->startX + (this->x - this->path->beginX) * this->path->cellSize;
		y = this->path->startY + (this->y - this->path->beginY) * this->path->cellSize;
	}
	void SetLocalXY(float x, float y) {
		//logprintf("SetLocalXY(%f,%f) = %f(%i)", x,y, x - this->path->startX, round((x - this->path->startX) / this->path->cellSize));
		this->x = this->path->beginX + (int)round((x - this->path->startX) / this->path->cellSize);
		this->y = this->path->beginY + (int)round((y - this->path->startY) / this->path->cellSize);
	}
	void SetG(int g) {
		this->G = g;
		this->F = this->GetF();
	}
	bool IsValid() {
		return (0 <= x && x < this->path->SIZE_X && 0 <= y && y < this->path->SIZE_Y);
	}
	bool IsClosed() {
		return this->closed;
	}
	bool IsOpen() {
		return (this->path->mapData->count(this->GetNode()) != 0);
	}
	bool IsBegining() {
		return (this->x == this->path->beginX && this->y == this->path->beginY);
	}
	mapPoint *GetParent() {
		return this->path->mapData->find(this->parent)->second;
	}
	int GetNode()
	{
		return XYToNode(this->x, this->y);
	}
	int GetF()
	{
		int H;
		if (this->path->type != PATH_TYPE_LINE) {
			H = abs(this->x - this->path->finalPoint->x) + abs(this->y - this->path->finalPoint->y);
		}
		return this->G + H;
	}
};