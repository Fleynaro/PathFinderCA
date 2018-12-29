#include "vehiclePath.h"


vehiclePath::vehiclePath()
{
	this->roadPath = new RoadPath();
	roadPath->Mode = RoadPath::mode::SMOOTH;
}


void vehiclePath::setStart_(Point3D *pos, float roadRadius)
{
	Point3D normalPoint;
	road startRoad = road::findNearbyRoad(pos, roadRadius, &normalPoint);
	if (!startRoad.isValid()) return;
	
	if (this->pathEnabled[0]) {
		this->firstPath = new Path();
		this->firstPath->startX = pos->getX();
		this->firstPath->startY = pos->getY();
		this->firstPath->startZ = pos->getZ();
		this->firstPath->endX = normalPoint.getX();
		this->firstPath->endY = normalPoint.getY();
		this->firstPath->endZ = normalPoint.getZ();
		this->firstPath->cellSize = 2.0;
	}

	this->roadPath->setStart(startRoad.getParentNode()->getId());
	this->roadPath->getSmoothPath()->setStartRoad(&startRoad);
}
void vehiclePath::setStart_(Point3D *pos, roadNode *node)
{
	if (this->pathEnabled[0]) {
		this->firstPath = new Path();
		this->firstPath->startX = pos->getX();
		this->firstPath->startY = pos->getY();
		this->firstPath->startZ = pos->getZ();
		this->firstPath->endX = node->getX();
		this->firstPath->endY = node->getY();
		this->firstPath->endZ = node->getZ();
		this->firstPath->cellSize = 2.0;
	}

	this->roadPath->setStart(node->getId());
}
void vehiclePath::setStart_(roadNode *node)
{
	this->roadPath->setStart(node->getId());
	this->firstPath = NULL;
}

void vehiclePath::setFinal_(Point3D *pos, float roadRadius)
{
	Point3D normalPoint;
	road finalRoad = road::findNearbyRoad(pos, roadRadius, &normalPoint);
	if (!finalRoad.isValid()) return;

	if (this->pathEnabled[1]) {
		this->lastPath = new Path();
		this->lastPath->startX = normalPoint.getX();
		this->lastPath->startY = normalPoint.getY();
		this->lastPath->startZ = normalPoint.getZ();
		this->lastPath->endX = pos->getX();
		this->lastPath->endY = pos->getY();
		this->lastPath->endZ = pos->getZ();
		this->lastPath->cellSize = 2.0;
	}

	this->roadPath->setFinal(finalRoad.getParentNode()->getId());
	this->roadPath->getSmoothPath()->setFinalRoad(&finalRoad);
}
void vehiclePath::setFinal_(Point3D *pos, roadNode *node)
{
	if (this->pathEnabled[1]) {
		this->lastPath = new Path();
		this->lastPath->startX = node->getX();
		this->lastPath->startY = node->getY();
		this->lastPath->startZ = node->getZ();
		this->lastPath->endX = pos->getX();
		this->lastPath->endY = pos->getY();
		this->lastPath->endZ = pos->getZ();
		this->lastPath->cellSize = 2.0;
	}

	this->roadPath->setFinal(node->getId());
}
void vehiclePath::setFinal_(roadNode *node)
{
	this->roadPath->setFinal(node->getId());
	this->lastPath = NULL;
}

void vehiclePath::Find()
{
	//logprintf("!!!---- startNode=%i, finalNode=%i, (%f,%f,%f; %f,%f,%f)", startNode->getId(), finalNode->getId(), start.getX(), start.getY(), start.getZ(), end.getX(), end.getY(), end.getZ());
	if (this->startNode != NULL) {
		if (this->start.isValid()) {
			this->setStart_(&this->start, this->startNode);
		}
		else {
			this->setStart_(this->startNode);
		}
	}
	else {
		this->setStart_(&this->start, this->roadRadius_);
	}

	if (this->finalNode != NULL) {
		if (this->end.isValid()) {
			this->setFinal_(&this->end, this->finalNode);
		}
		else {
			//logprintf(">>>> this->finalNode_");
			this->setFinal_(this->finalNode);
		}
	}
	else {
		this->setFinal_(&this->end, this->roadRadius_);
	}
	
	//logprintf(">>>> adding");
	if (this->firstPath != NULL) {
		combinedPath::addPath(this->firstPath);
	}
	if (this->roadPath != NULL) {
		combinedPath::addPath(this->roadPath);
	}
	if (this->lastPath != NULL) {
		combinedPath::addPath(this->lastPath);
	}
	//logprintf(">>>> finding");
	combinedPath::Find();
}


vehiclePath::~vehiclePath()
{
	if (this->firstPath != NULL) {
		delete this->firstPath;
	}
	if (this->roadPath != NULL) {
		delete this->roadPath;
	}
	if (this->lastPath != NULL) {
		delete this->lastPath;
	}
}
