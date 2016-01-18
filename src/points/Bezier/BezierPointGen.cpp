#include "BezierPointGen.hpp"
#include <points/PointsUItils.hpp>
#include <BezierPath.hpp>

BezierPointGen::BezierPointGen()
	: IPointGen() {
}

BezierPointGen::~BezierPointGen() {
}

void BezierPointGen::addControlPoints( const cc::Vec3f& cp ) {
	_controlPoints.push_back(cp);
}

void BezierPointGen::generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) const {
	printf("Generating bezier:\n");

	// get model's bounding box
	const BoundingBox& bbox = sourceModel.computeBoundingBox();

	std::vector<cc::Vec3f> bezierPoints;
	if( 4 == _controlPoints.size() ) { // all control points are provided
		printf("\tall points provided; building curve from points\n");
		bezierPoints = bezierPoints;
	} else if( 2 == _controlPoints.size() ) { // start and end points are provided; generate random midpoints
		printf("\tstart and end points provided; generating intermediate points\n");
		bezierPoints.push_back(_controlPoints[0]);
		bezierPoints.push_back(PointsUtils::randomPointInBbox(bbox));
		bezierPoints.push_back(PointsUtils::randomPointInBbox(bbox));
		bezierPoints.push_back(_controlPoints[1]);
	} else { // no control points provided (or too many); randomly generate all (p0 and p3 on faces, p1 and p2 randomly inside)
		printf("\tno points provided; generating all points\n");
		// get distance from one corner of the bbox to the other
		const cc::Vec3f cornerDiff = bbox.getCorner(BoundingBox::Corner::BottomLeftBack) - bbox.getCorner(BoundingBox::Corner::TopRightFront);
		const float minDistance = cc::math::percent<float>(cornerDiff.magnitude(), 75.0f);
		printf("\tsize: %f; minDistance: %f\n", cornerDiff.magnitude(), minDistance);

		// first point (on surface of bounding box)
		int p0Side = -1;
		const cc::Vec3f p0 = PointsUtils::randomPointOnBboxSide(bbox, p0Side, -1);
		bezierPoints.push_back(p0);
		printf("\tp0: face %d at (%f, %f, %f)\n", p0Side, p0.x, p0.y, p0.z);

		// second and third are randomly generated
		bezierPoints.push_back(PointsUtils::randomPointInBbox(bbox));
		printf("\tp1: (%f, %f, %f)\n", bezierPoints.back().x, bezierPoints.back().y, bezierPoints.back().z);
		bezierPoints.push_back(PointsUtils::randomPointInBbox(bbox));
		printf("\tp2: (%f, %f, %f)\n", bezierPoints.back().x, bezierPoints.back().y, bezierPoints.back().z);

		// fourth is on another side and must be a certain percentage of the size of the bounding box away from the first point
		// TODO: add an interation count, accepting last try if exceeding, to avoid infinite loop
		int p3Side = -1;
		cc::Vec3f p3;
		do {
			p3 = PointsUtils::randomPointOnBboxSide(bbox, p3Side, p0Side);
			printf("\t%s with distance %f\n", p3.distance(p0) < minDistance ? "FAILED" : "SUCCEEDED", p3.distance(p0));
		} while(p3.distance(p0) < minDistance);
		bezierPoints.push_back(p3);
		printf("\tp3: face %d at (%f, %f, %f)\n", p3Side, p3.x, p3.y, p3.z);	
	}

	// extract points along bezier curve
	BezierPath bezier;
	bezier.setControlPoints(bezierPoints);
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