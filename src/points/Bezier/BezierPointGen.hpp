#ifndef __bezier_point_gen__
#define __bezier_point_gen__

#include <vector>
#include <cc/Vec3.hpp>
#include <points/IPointGen.hpp>

class BezierPointGen : public IPointGen {
public:
	BezierPointGen();
	~BezierPointGen();

	void addControlPoints( const cc::Vec3f& cp );
	virtual void generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) const override;

private:
	std::vector<cc::Vec3f> _controlPoints;
};

#endif /* __bezier_point_gen__ */