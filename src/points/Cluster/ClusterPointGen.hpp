#ifndef __cluster_point_gen__
#define __cluster_point_gen__

#include <vector>
#include <cc/Vec3.hpp>
#include <points/IPointGen.hpp>

class ClusterPointGen : public IPointGen {
public:
	ClusterPointGen();
	~ClusterPointGen();

	virtual void setUserPoints( const std::vector<cc::Vec3f>& userPoints ) override;
	virtual void generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) override;

private:
	std::vector<cc::Vec3f> _primaryPoints;
};

#endif /* __cluster_point_gen__ */