#ifndef __voronoi_plane_generator__
#define __voronoi_plane_generator__

#include <cells/ICellGen.hpp>

class VoronoiCellGen : public ICellGen {
public:
	VoronoiCellGen();
	~VoronoiCellGen();

	virtual bool generate( const BoundingBox& bbox, const std::vector<cc::Vec3f>& samplePoints, std::vector<Cell>& outCells ) const override;
};

#endif /* __voronoi_plane_generator__ */