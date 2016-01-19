#ifndef __uniform_point_gen__
#define __uniform_point_gen__

#include <points/IPointGen.hpp>

class UniformPointGen : public IPointGen {
public:
	UniformPointGen();
	~UniformPointGen();

	virtual void setUserPoints( const std::vector<cc::Vec3f>& userPoints ) override;
	virtual void generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) override;
};

#endif /* __uniform_point_gen__ */