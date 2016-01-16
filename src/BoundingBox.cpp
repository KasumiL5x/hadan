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

cc::Vec3f BoundingBox::getCorner( Corner corner ) const {
	const float x = _halfExtents.x;
	const float y = _halfExtents.y;
	const float z = _halfExtents.z;
	cc::Vec3f point = _center;
	switch( corner ) {
		case Corner::TopLeftBack: {
			point += cc::Vec3f(-x, y, -z);
			break;
		}
		case Corner::TopLeftFront: {
			point += cc::Vec3f(-x, y, z);
			break;
		}
		case Corner::TopRightBack: {
			point += cc::Vec3f(x, y, -z);
			break;
		}
		case Corner::TopRightFront: {
			point += cc::Vec3f(x, y, z);
			break;
		}
		case Corner::BottomLeftBack: {
			point += cc::Vec3f(-x, -y, -z);
			break;
		}
		case Corner::BottomLeftFront: {
			point += cc::Vec3f(-x, -y, z);
			break;
		}
		case Corner::BottomRightBack: {
			point += cc::Vec3f(x, -y, -z);
			break;
		}
		case Corner::BottomRightFront: {
			point += cc::Vec3f(x, -y, z);
			break;
		}
	}
	return point;
}