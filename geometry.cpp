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
