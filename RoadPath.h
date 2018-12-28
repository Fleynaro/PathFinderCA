#pragma once


#include "genPath.h"
#include "geometry.h"
#include <limits>

#undef max

typedef unsigned short int nodeId;

#define MAX_ROAD_NODES 30000
#define ROAD_NOT 0xFFFF

struct roadNode;
struct roadPathNode;
struct road;

class RoadPath : public genPath
{
public:
	enum mode { DEFAULT, SMOOTH };
	RoadPath::mode Mode;
	static void fixRoads();
	static roadNode *getNode(nodeId index);
	static void addNode(nodeId index, roadNode node);
	static void removeNode(nodeId index);
	static int size() {
		return RoadPath::lastNodeId;
	};
	RoadPath();
	~RoadPath();

	void setStart(nodeId index) {
		startNode = getNode(index);
	}
	void setFinal(nodeId index) {
		finalNode = getNode(index);
	}
	void addPathNode(nodeId index, roadPathNode *pathNode);
	roadPathNode *getPathNode(roadNode *node, bool open = false);
	bool isPathNode(nodeId index);
	void openPathNodes(roadPathNode *parentNode, std::vector <roadPathNode*> *openNodes);

	void createPath(roadPathNode *node);
	void createSmoothPath(roadPathNode *node);
	void Find() override;
	void Destroy() override;
private:
	static roadNode nodes[MAX_ROAD_NODES];
	static nodeId lastNodeId;
	std::map<nodeId, roadPathNode*> pathNodes;
	roadNode *startNode;
	roadNode *finalNode;
};

struct roadNode
{
public:
	roadNode() {}
	roadNode(nodeId id, float x, float y, float z)
	{
		this->id = id;
		this->x = x;
		this->y = y;
		this->z = z;
		for (int i = 0; i < 4; i++) {
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
	void setPos(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
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
	bool isValid() {
		return this->getId() != ROAD_NOT;
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
	float getAngleXY(roadNode *node) {
		return atan2(node->getY() - this->getY(), node->getX() - this->getX());
	}
	void setChild(int index, nodeId index2) {
		this->child[index] = index2;
	}
	nodeId *getChilds() {
		return this->child;
	}
	nodeId getId() {
		return this->id;
	}
	roadNode *getChild(int index) {
		if (!this->isChild(index)) return NULL;
		return RoadPath::getNode(this->child[index]);
	}
	int getChildCount() {
		int count;
		while (count <= 3 && this->isChild(count)) count++;
		return count;
	}

	road getMultipleNode(int child, float &distance);
	road getMultipleNode(int child, roadNode *exclude = NULL);
	static roadNode *getInvisibleNode(std::vector <Point3D*> *points, roadNode *node, roadNode *parentNode = NULL, int level = 0);
	bool isInvisible(std::vector <Point3D*> *points, int world = 0);
private:
	nodeId id = ROAD_NOT;
	float x, y, z;
	nodeId child[4];
};


struct roadPathNode
{
public:
	roadPathNode(roadNode *node, roadPathNode *parentNode)
	{
		this->node = node;
		this->setParent(parentNode);
	}

	bool isOpen() {
		return this->getParent() != NULL;
	}
	bool isClosed() {
		return this->closed;
	}
	void close(std::vector <roadPathNode*> *openNodes) {
		this->closed = true;
		openNodes->erase(std::remove(openNodes->begin(), openNodes->end(), this), openNodes->end());
	}
	float getDist() {
		return this->dist;
	}
	float getF(roadPathNode *node) {
		return this->getDist() + this->getNode()->getDist(node->getNode());
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
	roadNode *getNode() {
		return this->node;
	}
private:
	roadNode * node;
	roadPathNode *parentNode;
	bool closed = false;
	float dist = 0.0;
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
		return (this->parentNode != NULL && this->nextNode != NULL) && (this->parentNode->isValid() && this->nextNode->isValid());
	}
	cell get() {
		if (!this->isValid()) return ROAD_NOT;
		return this->getParentNode()->getId() | (this->getNextNode()->getId() << 16);
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
	Vector3D getVector() {
		return Vector3D(
			this->getNextNode()->getX() - this->getParentNode()->getX(),
			this->getNextNode()->getY() - this->getParentNode()->getY(),
			this->getNextNode()->getZ() - this->getParentNode()->getZ()
		);
	}
private:
	roadNode *parentNode;
	roadNode *nextNode;
};