#ifndef __isamplepointgen__
#define __isamplepointgen__

#include <vector>
#include <cc/Vec3.hpp>
#include <Model.hpp>

class IPointGen {
public:
	IPointGen() {
	}
	virtual ~IPointGen() {
	}

	virtual void generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outSamplePoints ) const=0;
};

#endif /* __isamplepointgen__ */