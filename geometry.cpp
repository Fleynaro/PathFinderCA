#include "geometry.h"



float geometry::angleToRelative(float angle)
{
	if (angle < -180.0) {
		if (angle < -540.0) {
			angle += float(360.0) * floor(-angle / float(360.0));
		}
		angle += 360;
	}
	else if (angle > 180.0) {
		if (angle > 540.0) {
			angle -= float(360.0) * floor(angle / float(360.0));
		}
		angle -= float(360.0);
	}
	return angle;
}

float geometry::differRelAngles(float alpha, float beta)
{
	if (abs(beta - alpha) <= 180.0) {
		return (alpha - beta);
	}
	else if (alpha > 0) {
		return (alpha - beta - float(360.0));
	}
	else {
		return (alpha - beta + float(360.0));
	}
}

float geometry::toDegrees(float angle)
{
	return angle * float(180.0) / geometry::PI();
}





float Vector3D::getAngleXY()
{
	return atan2(this->y, this->x);
}

float Vector3D::getAngleBetweenXY(Vector3D * vector)
{
	return geometry::differRelAngles(
		geometry::toDegrees(this->getAngleXY()),
		geometry::toDegrees(vector->getAngleXY())
	);
}

bool Line::isPointOnLine2D(Point3D * point)
{
	float r = (float)1.0, r2 = (float)0.8;
	Point3D point1 = this->getFirstPoint();
	Point3D point2 = this->getSecondPoint();
	float
		k = (point1.getY() - point2.getY()) / ((point2.getX() == point1.getX()) ? (float)0.0001 : point2.getX() - point1.getX()),
		b = point1.getY() + k * point1.getX();
	return (
		(abs(k) > 5.0 || abs(point->getY() - (-k * point->getX() + b)) <= r) &&
		(point1.getX() - r2 < point->getX() && point->getX() < point2.getX() + r2 || point1.getX() + r2 > point->getX() && point->getX() > point2.getX() - r2) &&
		(point1.getY() - r2 < point->getY() && point->getY() < point2.getY() + r2 || point1.getY() + r2 > point->getY() && point->getY() > point2.getY() - r2)
	);
}
