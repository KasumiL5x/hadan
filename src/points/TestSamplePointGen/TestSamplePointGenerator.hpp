#ifndef __testsamplepointgenerator__
#define __testsamplepointgenerator__

#include <points/IPointGenerator.hpp>

class TestSamplePointGenerator : public IPointGenerator {
public:
	TestSamplePointGenerator();
	~TestSamplePointGenerator();

	virtual void generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outSamplePoints ) const override;
};

#endif /* __testsamplepointgenerator__ */