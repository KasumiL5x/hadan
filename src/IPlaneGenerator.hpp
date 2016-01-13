#ifndef __iplanegenerator__
#define __iplanegenerator__

#include <vector>
#include <cc/Vec3.hpp>
#include "Plane.hpp"
#include "BoundingBox.hpp"

class IPlaneGenerator {
public:
	IPlaneGenerator() {
	}
	virtual ~IPlaneGenerator() {
	}

	/**
	 * Generates a list of cutting planes.
	 * @param[in]  samplePoints   Input sample points used to seed the plane generation.
	 * @param[out] outPlanes      All generated planes.
	 * @param[out] outPlaneCounts Indices representing the number of planes.
	 *
	 * Each number in outPlaneCounts represents the plane count that should be read for the current cell.
	 * For instance, [7, 6, 4] would mean 3 cells with 7, 6, and 4 planes, respectively.  outPlanes would be a size of 17.
	 */
	virtual void generatePlanes( const BoundingBox& bbox, const std::vector<cc::Vec3f>& samplePoints, std::vector<Plane>& outPlanes, std::vector<int>& outPlaneCounts ) const=0;
};

#endif /* __iplanegenerator__ */