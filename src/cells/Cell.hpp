#ifndef __cell__
#define __cell__

#include <vector>
#include <Plane.hpp>
#include <cc/Vec3.hpp>

class Cell {
public:
	Cell();

	void addPlane( const Plane& plane );
	void addPoint( const cc::Vec3f& point );
	void addCount( int count );
	void addIndex( int index );
	void addPlanePoints( const std::vector<cc::Vec3f>& points );

	unsigned int getPlaneCount() const;
	const std::vector<Plane>& getPlanes() const;
	const std::vector<cc::Vec3f>& getPoints() const;
	const std::vector<int>& getCounts() const;
	const std::vector<int>& getIndices() const;
	const std::vector<std::vector<cc::Vec3f>>& getPlanePoints() const;

private:
	std::vector<Plane> _planes;

	std::vector<std::vector<cc::Vec3f>> _planePoints;

	std::vector<cc::Vec3f> _points;
	std::vector<int> _counts; // number of indices per face, e.g. 3,4 would be 3 indices followed by 4 indices
	std::vector<int> _indices; // actual indices in the format [index*counts[0], index*counts[1], ...]
};

#endif /*  */