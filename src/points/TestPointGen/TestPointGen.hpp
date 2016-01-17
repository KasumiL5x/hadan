#ifndef __testsamplepointgenerator__
#define __testsamplepointgenerator__

#include <points/IPointGen.hpp>

class TestPointGen : public IPointGen {
public:
	TestPointGen();
	~TestPointGen();

	virtual void generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) const override;

private:
	cc::Vec3f randomPointInBounds( float minx, float maxx, float miny, float maxy, float minz, float maxz ) const;
};

#endif /* __testsamplepointgenerator__ */