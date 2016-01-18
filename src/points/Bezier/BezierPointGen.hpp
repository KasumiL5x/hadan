#ifndef __bezier_point_gen__
#define __bezier_point_gen__

#include <points/IPointGen.hpp>

class BezierPointGen : public IPointGen {
public:
	BezierPointGen();
	~BezierPointGen();

	virtual void generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) const override;
};

#endif /* __bezier_point_gen__ */