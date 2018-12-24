#pragma once


#include "genPath.h"
#include <cmath>
#include <limits>
#undef max

typedef unsigned short int nodeId;

#define MAX_ROAD_NODES 30000
#define ROAD_NOT (0xFFFF - 1)

struct roadNode;
struct roadPathNode;
struct road;

class RoadPath : public genPath
{
public:
	static void fixRoads();
	static roadNode getNode(int16_t index);
	static void addNode(int16_t index, roadNode node);
	static int size() {
		return sizeof(RoadPath::nodes) / sizeof(roadNode);
	};
	RoadPath(CA_API *api);
	~RoadPath();

	void addPathNode(nodeId index, roadPathNode *pathNode);
	roadPathNode *getPathNode(roadNode *node, bool open = false);
	bool isPathNode(nodeId index);
	std::vector <roadPathNode*> openPathNodes(roadPathNode *parentNode);

	void Find() override;
	void Destroy() override;
private:
	static roadNode nodes[MAX_ROAD_NODES];
	std::map<nodeId, roadPathNode*> pathNodes;
	roadNode *startNode;
	roadNode *finalNode;
};




struct roadPathNode
{
public:
	roadPathNode(RoadPath *path, roadNode *node, roadPathNode *parentNode)
	{
		this->path = path;
		this->node = node;
		this->setParent(parentNode);
	}

	bool isOpen() {
		return this->getParent() != NULL;
	}
	bool isClosed() {
		return this->closed;
	}
	void close() {
		this->closed = true;
	}
	float getDist() {
		return this->dist;
	}
	void setDist(float value) {
		this->dist = value;
	}
	roadPathNode *getParent() {
		return this->parentNode;
	}
	void setParent(roadPathNode *parentNode) {
		this->parentNode = parentNode;
	}
	bool isOpen() {
		return this->path->isPathNode(this->node);
	}
	roadNode *getNode() {
		return this->node;
	}
private:
	roadNode *node;
	roadPathNode *parentNode;
	RoadPath *path;
	bool closed = false;
	float dist = 0.0;
};

struct roadNode
{
public:
	roadNode(nodeId id, float x, float y, float z)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->z = z;
		for (int i = 0; i < sizeof(child) / sizeof(nodeId); i++) {
			this->setChild(i, ROAD_NOT);
		}
	}
	float getX() {
		return this->x;
	}
	float getY() {
		return this->y;
	}
	float getZ() {
		return this->z;
	}
	bool isMultiple() {
		return (this->isChild(2) || this->isChild(3)) || this->isEnd();
	}
	bool isEnd() {
		return !this->isChild(0) || !this->isChild(1);
	}
	bool isChild(int index) {
		return this->child[index] != ROAD_NOT;
	}
	float getDist2To(float x, float y, float z) {
		return (this->getX() - x) * (this->getX() - x) + (this->getY() - y) * (this->getY() - y) + (this->getZ() - z) * (this->getZ() - z);
	}
	float getDist2(roadNode *node) {
		return this->getDist2To(node->getX(), node->getY(), node->getZ());
	}
	float getDist(roadNode *node) {
		return sqrt(this->getDist2(node));
	}
	void setChild(int index, nodeId index2) {
		this->child[index] = index2;
	}
	nodeId getId() {
		return this->id;
	}
	roadNode *getChild(int index) {
		if (!this->isChild(index)) return NULL;
		return &RoadPath::getNode(this->child[index]);
	}
	int getChildCount() {
		int count;
		while (count <= 3 && this->isChild(count)) count++;
		return count;
	}

	road getMultipleNode(int child, float &distance);
	road getMultipleNode(int child, roadNode *exclude = NULL);
private:
	nodeId id;
	float x, y, z;
	nodeId child[4];
};


struct road
{
public:
	road(roadNode *parentNode, roadNode *nextNode) {
		this->parentNode = parentNode;
		this->nextNode = nextNode;
	}
	road(int x) {
		this->parentNode = NULL;
		this->nextNode = NULL;
	}
	bool isValid() {
		return this->parentNode != NULL;
	}
	roadNode *getParentNode() {
		return this->parentNode;
	}
	roadNode *getNextNode() {
		return this->nextNode;
	}
	std::queue<roadNode*> getNodesTo(roadNode *finalNode);
	road getNearbyMultipleNode();
	bool getNormalPoint(float X, float Y, float &x, float &y, float &z);
	static road findNearbyRoad(float x, float y, float z, float radius, float &fx, float &fy, float &fz, float minRadius = 0.0);
private:
	roadNode *parentNode;
	roadNode *nextNode;
};