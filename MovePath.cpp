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
