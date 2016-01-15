#include "Cell.hpp"

Cell::Cell() {
}

void Cell::addPlane( const Plane& plane ) {
	_planes.push_back(plane);
}

void Cell::setCenter( const cc::Vec3f& center ) {
	_center = center;
}

unsigned int Cell::getPlaneCount() const {
	return static_cast<unsigned int>(_planes.size());
}

const std::vector<Plane>& Cell::getPlanes() const {
	return _planes;
}

const cc::Vec3f& Cell::getCenter() const {
	return _center;
}