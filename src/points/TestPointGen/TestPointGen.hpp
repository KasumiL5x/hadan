#ifndef __testsamplepointgenerator__
#define __testsamplepointgenerator__

#include <points/IPointGen.hpp>

class TestPointGen : public IPointGen {
public:
	TestPointGen();
	~TestPointGen();

	virtual void setFlux( const float flux ) override;
	virtual void setUserPoints( const std::vector<cc::Vec3f>& userPoints ) override;
	virtual void generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) override;

private:
	cc::Vec3f randomPointInBounds( float minx, float maxx, float miny, float maxy, float minz, float maxz ) const;
	cc::Vec3f randomPointOnUnitSide( const BoundingBox& bb, int notOnThisSide, int& outChosenSide, const cc::Vec3f* lastPoint ) const;
};

#endif /* __testsamplepointgenerator__ */