#include "MovePath.h"




int MovePath::getPointByPos(Point3D * pos, int lastPoint) {
	for (int it = lastPoint; it != 0; --it)
	{
		movePathPoint *curPoint = this->getPoint(it);
		movePathPoint *nextPoint = this->getPoint(it - 1);
		if ( Line(curPoint, nextPoint).isPointOnLine2D(pos) ) {
			return it;
		}
	}
	return -1;
}

float MovePath::getDistBetweenPoints(int p1, int p2)
{
	float dist = 0.0;
	for (int it = p1; it != p2; ++it)
	{
		movePathPoint *curPoint = this->getPoint(it);
		movePathPoint *nextPoint = this->getPoint(it + 1);
		dist += Vector3D(curPoint, nextPoint).getLong();
	}
	return dist;
}
