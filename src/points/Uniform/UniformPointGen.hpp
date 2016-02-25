#ifndef __uniform_point_gen__
#define __uniform_point_gen__

#include <points/IPointGen.hpp>

class UniformPointGen : public IPointGen {
public:
	UniformPointGen();
	~UniformPointGen();

	virtual void generateSamplePoints( const BoundingBox& boundingBox, const PointGenInfo& info, std::vector<cc::Vec3f>& outPoints ) override;
};

#endif /* __uniform_point_gen__ */