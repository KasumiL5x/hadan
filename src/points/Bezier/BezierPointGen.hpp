#ifndef __bezier_point_gen__
#define __bezier_point_gen__

#include <vector>
#include <cc/Vec3.hpp>
#include <points/IPointGen.hpp>

class BezierPointGen : public IPointGen {
public:
	BezierPointGen();
	~BezierPointGen();

	virtual void setFlux( const float flux ) override;
	virtual void setUserPoints( const std::vector<cc::Vec3f>& userPoints ) override;
	virtual void generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) override;

private:
	std::vector<cc::Vec3f> _controlPoints;
	float _flux;
};

#endif /* __bezier_point_gen__ */