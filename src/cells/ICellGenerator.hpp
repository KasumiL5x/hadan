#ifndef __iplanegenerator__
#define __iplanegenerator__

#include <vector>
#include <cc/Vec3.hpp>
#include <Plane.hpp>
#include <BoundingBox.hpp>
#include "Cell.hpp"

class ICellGenerator {
public:
	ICellGenerator() {
	}
	virtual ~ICellGenerator() {
	}

	/**
	 * Generates a list of cutting cells.
	 * @param[in] bbox         BoundingBox to clamp cell generation to.
	 * @param[in] samplePoints Sample points used to seed cell generation.
	 * @param[out] outCells    Vector of generated cutting Cells.
	 * @returns True upon success; false otherwise.
	 */
	virtual bool generate( const BoundingBox& bbox, const std::vector<cc::Vec3f>& samplePoints, std::vector<Cell>& outCells ) const=0;
};

#endif /* __iplanegenerator__ */