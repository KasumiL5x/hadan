// http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/
#ifndef __bezier_path__
#define __bezier_path__

#include <vector>
#include <cc/Vec3.hpp>

class BezierPath {
public:
	BezierPath();
	~BezierPath();

	void setControlPoints( const std::vector<cc::Vec3f>& newControlPoints );
	const std::vector<cc::Vec3f> getControlPoints() const;
	void interpolate( const std::vector<cc::Vec3f>& segmentPoints, float scale );
	void samplePoints( const std::vector<cc::Vec3f>& sourcePoints, float minSqrDist, float maxSqrDist, float scale );
	cc::Vec3f calculateBezierPoint( int curveIndex, float t );
	std::vector<cc::Vec3f> getDrawingPoints( unsigned int samples );
	std::vector<cc::Vec3f> getDrawingPointsAlternate( unsigned int samples );
	std::vector<cc::Vec3f> getDrawingPointsRecursive( unsigned int samples );

private:
	cc::Vec3f calculateBezierPoint( float t, const cc::Vec3f& p0, const cc::Vec3f& p1, const cc::Vec3f& p2, const cc::Vec3f& p3 );
	std::vector<cc::Vec3f> findDrawingPoints( unsigned int curveIndex );
	int findDrawingPoints( unsigned int curveIndex, float t0, float t1, std::vector<cc::Vec3f>& pointList, unsigned int insertionIndex );

private:
	std::vector<cc::Vec3f> _controlPoints;
	unsigned int _curveCount;
};

#endif /* __bezier_path__ */