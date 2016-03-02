#include "Cell.hpp"

Cell::Cell() {
}

void Cell::addPlane( const Plane& plane ) {
	_planes.push_back(plane);
}

void Cell::addPoint( const cc::Vec3f& point ) {
	_points.push_back(point);
}

void Cell::addCount( int count ) {
	_counts.push_back(count);
}

void Cell::addIndex( int index ) {
	_indices.push_back(index);
}

void Cell::addPlanePoints( const std::vector<cc::Vec3f>& points ) {
	_planePoints.push_back(points);
}

unsigned int Cell::getPlaneCount() const {
	return static_cast<unsigned int>(_planes.size());
}

const std::vector<Plane>& Cell::getPlanes() const {
	return _planes;
}

const std::vector<cc::Vec3f>& Cell::getPoints() const {
	return _points;
}

const std::vector<int>& Cell::getCounts() const {
	return _counts;
}

const std::vector<int>& Cell::getIndices() const {
	return _indices;
}

const std::vector<std::vector<cc::Vec3f>>& Cell::getPlanePoints() const {
	return _planePoints;
}