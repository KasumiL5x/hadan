#ifndef __isamplepointgenerator__
#define __isamplepointgenerator__

#include <vector>
#include <cc/Vec3.hpp>
#include <Model.hpp>

class IPointGenerator {
public:
	IPointGenerator() {
	}
	virtual ~IPointGenerator() {
	}

	virtual void generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outSamplePoints ) const=0;
};

#endif /* __isamplepointgenerator__ */