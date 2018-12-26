#include "RoadPath.h"

roadNode RoadPath::nodes[MAX_ROAD_NODES];
nodeId RoadPath::lastNodeId;

RoadPath::RoadPath()
{
	this->PATH_SIZE = 1500;
}


roadPathNode *RoadPath::getPathNode(roadNode *node, bool open)
{
	auto i = this->pathNodes.find(node->getId());
	if (i != this->pathNodes.end()) {
		return i->second;
	} else {
		if (open) {
			roadPathNode *pathNode = new roadPathNode(this, node, NULL);
			this->addPathNode(node->getId(), pathNode);
			return pathNode;
		}
		else {
			return NULL;
		}
	}
}
void RoadPath::addPathNode(nodeId index, roadPathNode *pathNode)
{
	this->pathNodes.insert(std::make_pair(index, pathNode));
}
bool RoadPath::isPathNode(nodeId index)
{
	return (this->pathNodes.find(index) != this->pathNodes.end());
}


void RoadPath::Find()
{
	this->status = PROCESS;

	roadPathNode
		*currentNode = this->getPathNode(this->startNode, true),
		*finalNode = this->getPathNode(this->finalNode, true);
	while (currentNode != finalNode)
	{
		currentNode->close();

		float minDist = std::numeric_limits<float>::max();
		std::vector <roadPathNode*> nodes = this->openPathNodes(currentNode);
		roadPathNode* nextNode = NULL;
		for (const auto &node : nodes) {
			float dist = node->getNode()->getDist2(this->finalNode);
			if (dist < minDist) {
				minDist = dist;
				nextNode = node;
			}
		}
		if (nextNode != NULL) {
			currentNode = nextNode;
		}
		else {
			currentNode = nextNode->getParent();
			if (currentNode == NULL) {
				this->status = NOT_FOUND;
				return;
			}
		}

		if (this->pathNodes.size() > (size_t)this->PATH_SIZE) {
			this->status = NOT_FOUND;
			return;
		}
	}

	this->createPath(currentNode);
}

void RoadPath::createPath(roadPathNode *node)
{
	int i = 0;
	while (i < this->PATH_SIZE) {
		if ( node == NULL ) {
			this->status = FOUND;
			return;
		}
		
		this->pathData->push_back(new pathPoint(
			node->getNode()->getX(),
			node->getNode()->getY(),
			node->getNode()->getZ()
		));
		node = node->getParent();
		i++;
	}
	this->status = NOT_FOUND;
}

std::vector <roadPathNode*> RoadPath::openPathNodes(roadPathNode *parentNode)
{
	std::vector <roadPathNode*> nodes;

	float parentDist = parentNode->getDist();
	for (int i = 0; i < 4; i++) {
		if (parentNode->getNode()->isChild(i)) break;
		float dist = 0.0;
		road childRoad = parentNode->getNode()->getMultipleNode(i, dist);
		if (!childRoad.isValid()) continue;

		roadPathNode* node = this->getPathNode(childRoad.getParentNode(), true);
		
		float totalDist = parentDist + dist;
		if (!node->isClosed()) {
			if (!node->isOpen() || totalDist < node->getDist()) {
				node->setDist(totalDist);
				node->setParent(parentNode);
			}
		}
	}
	return nodes;
}



road roadNode::getMultipleNode(int child, float &distance)
{
	roadNode
		*parentNode = this,
		*childNode = this->getChild(child);
	while (childNode != NULL && childNode->isMultiple()) {
		distance += childNode->getDist(parentNode);

		if (childNode->getChild(0) != parentNode) {
			parentNode = childNode;
			childNode = childNode->getChild(0);
		} else {
			parentNode = childNode;
			childNode = childNode->getChild(1);
		}
	}
	distance += childNode->getDist(parentNode);
	
	if (childNode == NULL) {
		return road(0);
	}
	return road(childNode, parentNode);
}

road roadNode::getMultipleNode(int child, roadNode *excludeNode)
{
	roadNode
		*parentNode = this,
		*childNode = this->getChild(child);
	while (childNode != NULL && childNode->isMultiple()) {
		if (childNode == excludeNode) {
			return road(0);
		}
		if (childNode->getChild(0) != parentNode) {
			parentNode = childNode;
			childNode = childNode->getChild(0);
		}
		else {
			parentNode = childNode;
			childNode = childNode->getChild(1);
		}
	}
	
	if (childNode == NULL) {
		return road(0);
	}
	return road(childNode, parentNode);
}

road road::getNearbyMultipleNode()
{
	float dist1, dist2;
	road node1 = this->getParentNode()->getMultipleNode(0, dist1);
	if (node1.isValid()) {
		road node2 = this->getParentNode()->getMultipleNode(1, dist2);
		if (dist1 < dist2 || node2.isValid()) {
			return node1;
		}
		else {
			return node2;
		}
	}
	else {
		return *this;
	}
}

std::queue<roadNode*> road::getNodesTo(roadNode *finalNode)
{
	std::queue<roadNode*> nodes;
	roadNode
		*parentNode = this->getParentNode(),
		*childNode = this->getNextNode();
	while (childNode != NULL && childNode->isMultiple()) {
		nodes.push(childNode);
		if (childNode->getChild(0) != parentNode) {
			parentNode = childNode;
			childNode = childNode->getChild(0);
		}
		else {
			parentNode = childNode;
			childNode = childNode->getChild(1);
		}
	}
	return nodes;
}

bool road::getNormalPoint(float X, float Y, float &fx, float &fy, float &fz)
{
	roadNode
		*node1 = this->getParentNode(),
		*node2 = this->getNextNode();
	double
		k1 = (node2->getY() - node1->getY()) / (node1->getX() == node2->getX() ? 0.000000001 : (node1->getX() - node2->getX())),
		b1 = node1->getY() + node1->getX() * k1,
		k2 = -1 / (k1 == 0 ? 0.000000001 : k1),
		b2 = Y + X * k2;

	double x = (b2 - b1) / (k2 - k1);
	double y = -k1 * x + b1;
	double z = (node2->getZ() + node1->getZ()) / 2.;
	fx = (float)x;
	fy = (float)y;
	fz = (float)z;

	//logprintf("ROAD(%i, %i): %f < %f < %f && %f < %f < %f", node1->getId(), node2->getId(), node1->getX() - 0.0001, x, node2->getX() + 0.0001, node1->getY() - 0.0001, y, node2->getY() + 0.0001);
	return ((node1->getX() - 0.0001 <= x && x <= node2->getX() + 0.0001) || (node1->getX() - 0.0001 >= x && x >= node2->getX() + 0.0001)) && ((node1->getY() - 0.0001 <= y && y <= node2->getY() + 0.0001) || (node1->getY() - 0.0001 >= y && y >= node2->getY() + 0.0001));
}

road road::findNearbyRoad(float x, float y, float z, float radius, float &fx, float &fy, float &fz, float minRadius)
{
	float minDist = std::numeric_limits<float>::max();
	road Road(0);
	minRadius *= minRadius;
	radius *= radius;

	for (int i = 0, size = RoadPath::size(); i < size; i++) {
		roadNode *node = RoadPath::getNode(i);
		float dist = node->getDist2To(x, y, z);
		if (dist <= 50.0*50.0) {
			for (int j = 0; j < 4; j++) {
				roadNode *child = node->getChild(j);
				if (child == NULL) break;
				if (child->getId() > i)
				{
					float tx, ty, tz;
					road tempRoad = road(node, child);
					if (tempRoad.getNormalPoint(x, y, tx, ty, tz)) {
						float dist2 = (x - tx)*(x - tx) + (y - ty)*(y - ty) + (z - tz)*(z - tz);
						if (dist2 < minDist) {
							minDist = dist2;
							Road = tempRoad;
							fx = tx;
							fy = ty;
							fz = tz;
						}
					}
				}
			}
		}
		if (dist < minDist) {
			minDist = dist;
			Road = road(node, node);
			fx = node->getX();
			fy = node->getY();
			fz = node->getZ();
		}

		if (minDist >= minRadius && minDist < radius) {
			break;
		}
	}
	return Road;
}



void RoadPath::fixRoads()
{
	std::vector<int> passed;
	for (int i = 0, size = RoadPath::size(); i < size; i++) {
		roadNode *node = RoadPath::getNode(i);
		if (node == NULL) continue;
		
		if ( node->isMultiple() ) {
			passed.push_back(i);
			for (int j = 0; j < 4; j++) {
				roadNode *child = node->getMultipleNode(j).getParentNode();
				if (child == NULL || std::find(passed.begin(), passed.end(), child->getId()) != passed.end()) continue;

				roadNode
					*parentNode = node,
					*childNode = node->getChild(j);
				while (childNode != NULL && childNode != child)
				{
					if (childNode->getChild(0) == parentNode) {
						int temp = childNode->getChild(1)->getId();
						childNode->setChild(1, parentNode->getId());
						childNode->setChild(0, temp);
						parentNode = childNode;
						childNode = childNode->getChild(0);
					}
					else {
						parentNode = childNode;
						childNode = childNode->getChild(0);
					}
				}
			}
		}

		float x, y, z;
		api->CA_RayCastLine(node->getX(), node->getY(), node->getZ() + (float)1.5, node->getX(), node->getY(), node->getZ() - (float)3.0, x, y, z, 0);
		node->setPos(x, y, z);
	}
}

roadNode *RoadPath::getNode(nodeId index)
{
	return &RoadPath::nodes[index];
}

void RoadPath::addNode(nodeId index, roadNode node)
{
	RoadPath::nodes[index] = node;
	if (index >= RoadPath::lastNodeId) {
		RoadPath::lastNodeId = index + 1;
	}
}

void RoadPath::removeNode(nodeId index)
{
	RoadPath::nodes[index] = roadNode();
	if (index == RoadPath::lastNodeId - 1) {
		RoadPath::lastNodeId --;
	}
}

void RoadPath::Destroy()
{
	delete this;
}

RoadPath::~RoadPath()
{
	for (auto &pNode : this->pathNodes) {
		delete pNode.second;
	}
}
