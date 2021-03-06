#include "RoadPath.h"

roadNode RoadPath::nodes[MAX_ROAD_NODES];
nodeId RoadPath::lastNodeId;

RoadPath::RoadPath()
{
	this->PATH_SIZE = 20000;
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
	if (this->status != NOT_STARTED) return;
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
	this->setGenDist(finalNode->getDist());

	//logprintf("PATH FORMED. SIZE %i, last id = %i", this->pathNodes.size(), currentNode->getNode()->getId());
	switch (this->Mode)
	{
		case mode::DEFAULT: this->createPath(currentNode); break;
		case mode::SMOOTH: this->createSmoothPath(currentNode); break;
	}
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

RoadPath::smoothPath *RoadPath::getSmoothPath()
{
	if (this->SmoothPath == NULL) {
		this->SmoothPath = new smoothPath();
	}
	return this->SmoothPath;
}
void RoadPath::createSmoothPath(roadPathNode *node)
{
	std::deque <roadPathNode*> smoothPath;
	int i = 0;
	while (i < this->PATH_SIZE) {
		if (node == NULL) {
			this->status = FOUND;
			break;
		}
		//logprintf("----->(%i) node = %i", i, node->getNode()->getId());
		smoothPath.push_back(node);
		node = node->getParent();
		i++;
	}

	if (i == this->PATH_SIZE || i <= 2) {
		this->status = NOT_FOUND;
		return;
	}

	if (getSmoothPath()->getStartRoad()->isValid()) {
		roadNode* startSecondNode = (*(smoothPath.end() - 2))->getNode();
		if (startSecondNode == getSmoothPath()->getStartRoad()->getNextNode()) {
			smoothPath.pop_back();
			if (--i == 2) {
				this->status = NOT_FOUND;
				return;
			}
		}
	}
	if (!getSmoothPath()->getFinalRoad()->isValid() || (*(smoothPath.begin() + 1))->getNode() != getSmoothPath()->getFinalRoad()->getNextNode()) {
		smoothPath.push_front(*smoothPath.begin());
		i++;
	}
	
	roadPathNode *firstNode, *middleNode;
	firstNode = smoothPath.back();
	smoothPath.pop_back();
	middleNode = smoothPath.back();
	smoothPath.pop_back();
	this->pathData->push_back(new pathPoint(
		firstNode->getNode()->getX(),
		firstNode->getNode()->getY(),
		firstNode->getNode()->getZ()
	));
	

	while (!smoothPath.empty())
	{
		roadPathNode *lastNode = smoothPath.back();
		road
			firstRoad(firstNode->getNode(), middleNode->getNode()),
			secondRoad(middleNode->getNode(), lastNode->getNode());
		if (firstRoad.isValid() && secondRoad.isValid())
		{
			float betweenAngle = abs(firstRoad.getVector().getAngleBetweenXY(&secondRoad.getVector()));
			if (betweenAngle > 90.0) { //finding the acute angle
				betweenAngle = (float)180.0 - betweenAngle;
			}
			//logprintf("------>firstNode = %i, middleNode=%i, lastNode=%i: betweenAngle=%f", firstNode->getNode()->getId(), middleNode->getNode()->getId(), lastNode->getNode()->getId(), betweenAngle);
			if (betweenAngle > 30.0 && middleNode != lastNode)
			{
				road roadVector(firstNode->getNode(), lastNode->getNode()); //create a vector as a road
				float
					alpha = firstRoad.getVector().getAngleXY() + geometry::PI(),
					delta = roadVector.getVector().getAngleXY(),
					vectorLong = roadVector.getVector().getLongXY(), //long of a vector created by lastNode and firstNode
					relNX = cos(delta - alpha) * vectorLong,
					relNY = sin(delta - alpha) * vectorLong;
				short int
					relNXsign = geometry::sign(relNX),
					relNYsign = geometry::sign(relNY);
				relNX = abs(relNX),
				relNY = abs(relNY);

				double square = relNX / 3.0;
				if (square > 3.0) {
					square = 3.0;
				}
				float
					b = float(log(relNY / square) / log(relNX / (relNX - square))),
					a = relNY / pow(relNX, b);
				if (a == 0) a = (float)0.001;
				//logprintf("--------->relNX=%f,relNY=%f(sign = %i,%i) vectorLong=%f, alpha=%f,delta=%f; a=%f,b=%f", relNX, relNY, relNXsign, relNYsign, vectorLong, geometry::toDegrees(alpha), geometry::toDegrees(delta), a, b);

				const float step = (float)1.3;
				float
					rx = step,
					rdist = 0.0;
				while (rx < relNX) {
					float
						xb = pow(rx, b),
						ry = a * xb;
					if (ry > relNY) break;
					float f = ry - pow(rx - step, b) * a;
					rdist += sqrt(step*step + f*f);
					//logprintf("!!! ------------>>> rx=%f, xb=%f, ry=%f, rdist=%f", rx, xb, ry, rdist);
					if (rdist >= 2.5) {
						float
							rv = sqrt(rx * rx + ry * ry),
							beta = atan2(ry * relNYsign, rx * relNXsign),
							wx = firstNode->getNode()->getX() + rv * cos(beta + alpha),
							wy = firstNode->getNode()->getY() + rv * sin(beta + alpha),
							wz = firstNode->getNode()->getZ() + (lastNode->getNode()->getZ() - firstNode->getNode()->getZ()) * rx / relNX;

						API::Vector point;
						if (API::CA::getInstance()->performRayTest(API::Vector(wx, wy, wz + 10.0f), API::Vector(wx, wy, wz - 10.0f), point, 0) != 0) {
							this->pathData->push_front(new pathPoint(point.x, point.y, point.z));
							//logprintf("ADD -------------> wx, wy, wz = %f,%f,%f (rx = %f/%f, ry = %f/%f, a = %f, b = %f) rdist = %f", wx, wy, wz, rx, relNX, ry, relNY, a, b, rdist);
						}
					}
					else if (rdist > 20.0) {
						//logprintf("------------>>> break for rx=%f,ry=%f (dist = %f)", rx, ry, rdist);
						break;
					}
					rx += step;
				}
				firstNode = middleNode;
				middleNode = lastNode;
				smoothPath.pop_back();
				if (smoothPath.empty()) {
					break;
				}
				lastNode = smoothPath.back();
			}
			else {
				const float
					height = float(5.0), //height
					deltaR = float(5.0), //max dist between nodes
					k = float(0.7); //���� ������ �������� �� 0.7x � 1.0x, ��� 0.7x = kx

				float parts = firstRoad.getVector().getLong2() / (deltaR * deltaR);
				if (parts > geometry::sq(float(1.0) + k)) {
					parts = sqrt(parts);
					//logprintf("---------->parts=%f", parts);
					float j = 1.0;
					while (j + k < parts) {
						Vector3D v = firstRoad.getVector();
						float
							wx = firstNode->getNode()->getX() + v.getX() / parts * j,
							wy = firstNode->getNode()->getY() + v.getY() / parts * j,
							wz = firstNode->getNode()->getZ() + v.getZ() / parts * j;

						API::Vector point;
						if (API::CA::getInstance()->performRayTest(API::Vector(wx, wy, wz + height), API::Vector(wx, wy, wz - height), point, 0) != 0) {
							this->pathData->push_front(new pathPoint(point.x, point.y, point.z));
						}
						j += 1.0;
					}
				}
				this->pathData->push_front(new pathPoint(
					middleNode->getNode()->getX(),
					middleNode->getNode()->getY(),
					middleNode->getNode()->getZ()
				));
				//logprintf("ADD ------------->x=%f,y=%f,z=%f", middleNode->getNode()->getX(), middleNode->getNode()->getY(), middleNode->getNode()->getZ());
			}
		}
		
		firstNode = middleNode;
		middleNode = lastNode;
		smoothPath.pop_back();
	}
}

void RoadPath::smoothPath::setStartRoad(road *Road)
{
	this->startRoad = *Road;
}
road *RoadPath::smoothPath::getStartRoad() {
	return &this->startRoad;
}

void RoadPath::smoothPath::setFinalRoad(road * Road)
{
	this->finalRoad = *Road;
}

road * RoadPath::smoothPath::getFinalRoad()
{
	return &this->finalRoad;
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



roadNode * roadNode::getChild(int index) {
	if (!this->isChild(index)) return NULL;
	return RoadPath::getNode(this->child[index]);
}

bool roadNode::isValid(nodeId node) {
	return 0 <= node && node < RoadPath::size();
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
		return road();
	}

	distance += childNode->getDist(parentNode);
	return road(childNode, parentNode);
}

road roadNode::getNearbyMultipleNode()
{
	float dist1, dist2;
	road node1 = this->getMultipleNode(0, dist1);
	road node2 = this->getMultipleNode(1, dist2);
	if (node1.isValid() && node2.isValid()) {
		if (dist1 < dist2) {
			return node1;
		}
		else {
			return node2;
		}
	}
	else {
		return road(this, NULL);
	}
}

road roadNode::getMultipleNode(int child, roadNode *excludeNode)
{
	roadNode
		*parentNode = this,
		*childNode = this->getChild(child);
	while (childNode != NULL && !childNode->isMultiple()) {
		if (childNode == excludeNode) {
			return road();
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
		return road();
	}
	return road(childNode, parentNode);
}

roadNode * roadNode::getInvisibleNode(std::vector <Point3D> *points, roadNode * node, roadNode * parentNode, int level)
{
	for (int i = 0; i < 4; i++) {
		if (!node->isChild(i)) break;
		roadNode *child = node->getMultipleNode(i).getParentNode();
		if (child == NULL || !child->isValid()) {
			continue;
		}
		if (child != parentNode) {
			if (level >= 2 && child->isInvisible(points, 0)) {
				return child;
			}
			else {
				roadNode * node2 = getInvisibleNode(points, child, node, level + 1);
				if (node2 != NULL) {
					return node2;
				}
			}
		}
	}

	return NULL;
}

bool roadNode::isInvisible(std::vector <Point3D> *points, int world)
{
	for (auto point : *points) {
		if (point.getWorld() != world) continue;
		API::Vector point2;
		if (API::CA::getInstance()->performRayTest(API::Vector(this->getX(), this->getY(), this->getZ() + 1.0f), API::Vector(point.getX(), point.getY(), point.getZ() + 1.0f), point2, 0) == 0) {
			return false;
		}
	}
	return true;
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

bool road::getNormalPoint(float X, float Y, Point3D *normal)
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
	normal->setPos((float)x, (float)y, (float)z);

	//logprintf("ROAD(%i, %i): %f < %f < %f && %f < %f < %f", node1->getId(), node2->getId(), node1->getX() - 0.0001, x, node2->getX() + 0.0001, node1->getY() - 0.0001, y, node2->getY() + 0.0001);
	return ((node1->getX() - 0.0001 <= x && x <= node2->getX() + 0.0001) || (node1->getX() - 0.0001 >= x && x >= node2->getX() + 0.0001)) && ((node1->getY() - 0.0001 <= y && y <= node2->getY() + 0.0001) || (node1->getY() - 0.0001 >= y && y >= node2->getY() + 0.0001));
}

road road::findNearbyRoad(Point3D *pos, float radius, Point3D *normal, float minRadius)
{
	float minDist = std::numeric_limits<float>::max();
	road Road;
	minRadius *= minRadius;
	radius *= radius;

	for (int i = 0, size = RoadPath::size(); i < size; i++) {
		roadNode *node = RoadPath::getNode(i);
		if (!node->isValid()) continue;
		float dist = node->getDist2To(pos);
		if (dist <= 50.0*50.0) {
			for (int j = 0; j < 4; j++) {
				roadNode *child = node->getChild(j);
				if (child == NULL) break;
				if (child->getId() > i)
				{
					Point3D normalPoint;
					road tempRoad = road(node, child);
					if (tempRoad.getNormalPoint(pos->getX(), pos->getY(), &normalPoint)) {
						float dist2 = Vector3D(pos, &normalPoint).getLong2();
						if (dist2 < minDist) {
							minDist = dist2;
							Road = tempRoad;
							*normal = normalPoint;
						}
					}
				}
			}
		}
		if (dist < minDist) {
			minDist = dist;
			Road = road(node, node);
			*normal = node->getPos();
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
		
		API::Vector point;
		if (API::CA::getInstance()->performRayTest(API::Vector(node->getX(), node->getY(), node->getZ() + 1.5f), API::Vector(node->getX(), node->getY(), node->getZ() - 20.0f), point, 0) != 0) {
			node->setPos(&Point3D(point.x, point.y, point.z));
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
	if (this->SmoothPath != NULL) {
		delete this->SmoothPath;
	}
}
