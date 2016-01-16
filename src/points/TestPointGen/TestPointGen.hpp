#ifndef __testsamplepointgenerator__
#define __testsamplepointgenerator__

#include <points/IPointGen.hpp>

class TestPointGen : public IPointGen {
public:
	TestPointGen();
	~TestPointGen();

	virtual void generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) const override;
};

#endif /* __testsamplepointgenerator__ */