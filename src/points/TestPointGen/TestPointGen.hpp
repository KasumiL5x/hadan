#ifndef __testsamplepointgenerator__
#define __testsamplepointgenerator__

#include <points/IPointGen.hpp>

class TestPointGen : public IPointGen {
public:
	TestPointGen();
	~TestPointGen();

	virtual void generateSamplePoints( const BoundingBox& boundingBox, const PointGenInfo& info, std::vector<cc::Vec3f>& outPoints ) override;
};

#endif /* __testsamplepointgenerator__ */