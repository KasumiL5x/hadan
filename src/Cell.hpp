#ifndef __cell__
#define __cell__

#include <vector>
#include <cc/Vec3.hpp>
#include "Plane.hpp"

class Cell {
public:
	Cell();

	void addPlane( const Plane& plane );
	void setCenter( const cc::Vec3f& center );

	unsigned int getPlaneCount() const;
	const std::vector<Plane>& getPlanes() const;
	const cc::Vec3f& getCenter() const;

private:
	std::vector<Plane> _planes;
	cc::Vec3f _center;
};

#endif /*  */