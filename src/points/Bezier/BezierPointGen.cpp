#include "BezierPointGen.hpp"
#include <points/PointsUItils.hpp>
#include <BezierPath.hpp>

BezierPointGen::BezierPointGen()
	: IPointGen() {
}

BezierPointGen::~BezierPointGen() {
}

void BezierPointGen::generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) const {
	const BoundingBox& bbox = sourceModel.computeBoundingBox();

	printf("Generating bezier:\n");

	// get distance from one corner of the bbox to the other
	const cc::Vec3f cornerDiff = bbox.getCorner(BoundingBox::Corner::BottomLeftBack) - bbox.getCorner(BoundingBox::Corner::TopRightFront);
	const float minDistance = cc::math::percent<float>(cornerDiff.magnitude(), 75.0f);

	printf("\tsize: %f; minDistance: %f\n", cornerDiff.magnitude(), minDistance);

	// compute first point of bezier
	int firstSide = -1;
	const cc::Vec3f p0 = PointsUtils::randomPointOnBboxSide(bbox, firstSide, -1);
	printf("\tp0: face %d at (%f, %f, %f)\n", firstSide, p0.x, p0.y, p0.z);

	// compute two random points inside the bounding box
	const cc::Vec3f p1 = PointsUtils::randomPointInBbox(bbox);
	const cc::Vec3f p2 = PointsUtils::randomPointInBbox(bbox);
	printf("\tp1: (%f, %f, %f)\n", p1.x, p1.y, p1.z);
	printf("\tp2: (%f, %f, %f)\n", p2.x, p2.y, p2.z);

	// compute last point of bezier
	int lastSide = -1;
	cc::Vec3f p3;
	do {
		p3 = PointsUtils::randomPointOnBboxSide(bbox, lastSide, firstSide);
		printf("\t%s with distance %f\n", p3.distance(p0) < minDistance ? "FAILED" : "SUCCEEDED", p3.distance(p0));
	} while(p3.distance(p0) < minDistance);

	printf("\tp3: face %d at (%f, %f, %f)\n", lastSide, p3.x, p3.y, p3.z);	

	// extract points along bezier curve
	std::vector<cc::Vec3f> controlPoints;
	controlPoints.push_back(p0);
	controlPoints.push_back(p1);
	controlPoints.push_back(p2);
	controlPoints.push_back(p3);
	BezierPath bezier;
	bezier.setControlPoints(controlPoints);
	for( auto& pnt : bezier.getDrawingPoints() ) {
		outPoints.push_back(pnt);
	}

	// fluctuate points from curve to break how uniform they appear
	const float FLUX = 0.05f;
	Random<float, int> rnd;
	for( auto& pnt : outPoints ) {
		pnt += cc::Vec3f(rnd.nextReal(-FLUX, FLUX), rnd.nextReal(-FLUX, FLUX), rnd.nextReal(-FLUX, FLUX));
	}

	// add some uniformly random points to add some extra detail away from the curve
	for( unsigned int i = 0; i < pointCount; ++i ) {
		outPoints.push_back(PointsUtils::randomPointInBbox(bbox));
	}

	fflush(stdout);
}