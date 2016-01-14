#ifndef __voronoi_plane_generator__
#define __voronoi_plane_generator__

#include "IPlaneGenerator.hpp"

class VoronoiPlaneGenerator : public IPlaneGenerator {
public:
	VoronoiPlaneGenerator();
	~VoronoiPlaneGenerator();

	virtual void generatePlanes( const BoundingBox& bbox, const std::vector<cc::Vec3f>& samplePoints, std::vector<Plane>& outPlanes, std::vector<int>& outPlaneCounts, std::vector<cc::Vec3f>& outCellPositions ) const override;
};

#endif /* __voronoi_plane_generator__ */