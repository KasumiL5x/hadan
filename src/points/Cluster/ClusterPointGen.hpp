#ifndef __cluster_point_gen__
#define __cluster_point_gen__

#include <points/IPointGen.hpp>

class ClusterPointGen : public IPointGen {
public:
	ClusterPointGen();
	~ClusterPointGen();

	virtual void generateSamplePoints( const Model& sourceModel, const PointGenInfo& info, std::vector<cc::Vec3f>& outPoints ) override;
};

#endif /* __cluster_point_gen__ */