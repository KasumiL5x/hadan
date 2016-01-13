#include "BoundingBox.hpp"

BoundingBox::BoundingBox() {
}

BoundingBox::BoundingBox( const cc::Vec3f& center, const cc::Vec3f& halfExtents )
	: _center(center), _halfExtents(halfExtents) {
}

const cc::Vec3f& BoundingBox::getCenter() const {
	return _center;
}

const cc::Vec3f& BoundingBox::getHalfExtents() const {
	return _halfExtents;
}

float BoundingBox::minX() const {
	return _center.x - _halfExtents.x;
}

float BoundingBox::minY() const {
	return _center.y - _halfExtents.y;
}

float BoundingBox::minZ() const {
	return _center.z - _halfExtents.z;
}

float BoundingBox::maxX() const {
	return _center.x + _halfExtents.x;
}

float BoundingBox::maxY() const {
	return _center.y + _halfExtents.y;
}

float BoundingBox::maxZ() const {
	return _center.z + _halfExtents.z;
}