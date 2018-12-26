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
			roadPathNode *pathNode = new roadPathNode(node, NULL);
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
	//logprintf("RoadPath::Find(): currentNode = %i, finalNode=%i(%i)", currentNode->getNode()->getId(), finalNode->getNode()->getId(), finalNode->getNode()->isMultiple());
	std::vector <roadPathNode*> openNodes;
	while (currentNode != finalNode)
	{
		currentNode->close(&openNodes);

		float minF = std::numeric_limits<float>::max();
		//logprintf("--------- currentNode = %i", currentNode->getNode()->getId());
		this->openPathNodes(currentNode, &openNodes);
		roadPathNode* nextNode = NULL;
		for (const auto &node : openNodes) {
			float f = node->getF(finalNode);
			//logprintf("-------------- node = %i, f=%f(dist=%f,d2=%f)", node->getNode()->getId(), f, node->getDist(), node->getNode()->getDist(finalNode->getNode()));
			if (f < minF) {
				minF = f;
				nextNode = node;
			}
		}
		//logprintf("--------- nodes.size() = %i, minF=%f", nodes.size(), minF);
		if (nextNode != NULL) {
			currentNode = nextNode;
		}
		else {
			if (currentNode == NULL) {
				this->status = NOT_FOUND;
				return;
			}
			currentNode = currentNode->getParent();
			//logprintf("--------- go back to %i", currentNode->getParent()->getNode()->getId());
		}

		if (this->pathNodes.size() > (size_t)this->PATH_SIZE) {
			this->status = NOT_FOUND;
			return;
		}
	}

	//logprintf("PATH FORMED. SIZE %i, last id = %i", this->pathNodes.size(), currentNode->getNode()->getId());
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

void RoadPath::createSmoothPath(roadPathNode *node)
{
	int i = 0;
	while (i < this->PATH_SIZE) {
		if (node == NULL) {
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

void RoadPath::openPathNodes(roadPathNode *parentNode, std::vector <roadPathNode*> *openNodes)
{
	float parentDist = parentNode->getDist();
	for (int i = 0; i < 4; i++) {
		if (!parentNode->getNode()->isChild(i)) break;
		roadNode *childNode = parentNode->getNode()->getChild(i);
		//logprintf("------------ child = %i (parentDist=%f)", i, parentDist);

		roadPathNode* node = this->getPathNode(childNode, true);
		float totalDist = parentDist + childNode->getDist(parentNode->getNode());
		if (!node->isClosed()) {
			if (!node->isOpen() || totalDist < node->getDist()) {
				if (!node->isOpen()) {
					openNodes->push_back(node);
				}
				node->setDist(totalDist);
				node->setParent(parentNode);
			}
			//logprintf("---------------> nodeId = %i, totalDist=%f(dist=%f) (parentNode=%i)", node->getNode()->getId(), totalDist, childNode->getDist(parentNode->getNode()), parentNode->getNode()->getId());
		}
	}
}



road roadNode::getMultipleNode(int child, float &distance)
{
	roadNode
		*parentNode = this,
		*childNode = this->getChild(child);
	while (childNode != NULL && !childNode->isMultiple()) {
		distance += childNode->getDist(parentNode);

		if (childNode->getChild(0) != parentNode) {
			parentNode = childNode;
			childNode = childNode->getChild(0);
		} else {
			parentNode = childNode;
			childNode = childNode->getChild(1);
		}
	}
	if (childNode == NULL) {
		return road(0);
	}

	distance += childNode->getDist(parentNode);
	return road(childNode, parentNode);
}

road roadNode::getMultipleNode(int child, roadNode *excludeNode)
{
	roadNode
		*parentNode = this,
		*childNode = this->getChild(child);
	while (childNode != NULL && !childNode->isMultiple()) {
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
		if (api->CA_RayCastLine(node->getX(), node->getY(), node->getZ() + (float)1.5, node->getX(), node->getY(), node->getZ() - (float)20.0, x, y, z, 0) != 0) {
			node->setPos(x, y, z);
		}
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
