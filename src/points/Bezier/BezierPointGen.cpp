#include "BezierPointGen.hpp"
#include "BezierPath.hpp"
#include "../../MTLog.hpp"
#include <Random.hpp>

BezierPointGen::BezierPointGen()
	: IPointGen() {
}

BezierPointGen::~BezierPointGen() {
}

void BezierPointGen::generateSamplePoints( const BoundingBox& boundingBox, const PointGenInfo& info, std::vector<cc::Vec3f>& outPoints ) {
	MTLog::instance()->log("Generating bezier:\n");

	if( 0 == info.samples ) {
		MTLog::instance()->log("Warning: Curve will be ignored as Samples were zero.  Fracturing will not occur.\n");
		return;
	}

	// create seeded random
	Random<float, int> rnd(info.seed);

	// set control points to user-provided points
	std::vector<cc::Vec3f> controlPoints = info.userPoints;

	std::vector<cc::Vec3f> bezierPoints;
	if( 4 == controlPoints.size() ) { // all control points are provided
		MTLog::instance()->log("\tall points provided; building curve from points\n");
		bezierPoints = controlPoints;
	} else if( 2 == controlPoints.size() ) { // start and end points are provided; generate random midpoints
		MTLog::instance()->log("\tstart and end points provided; generating intermediate points\n");
		bezierPoints.push_back(controlPoints[0]);
		bezierPoints.push_back(rnd.pointInBBox(boundingBox));
		bezierPoints.push_back(rnd.pointInBBox(boundingBox));
		bezierPoints.push_back(controlPoints[1]);
	} else { // no control points provided (or too many); randomly generate all (p0 and p3 on faces, p1 and p2 randomly inside)
		MTLog::instance()->log("\tno points provided; generating all points\n");
		// get distance from one corner of the bbox to the other
		const cc::Vec3f cornerDiff = boundingBox.getCorner(BoundingBox::Corner::BottomLeftBack) - boundingBox.getCorner(BoundingBox::Corner::TopRightFront);
		const float minDistance = cc::math::percent<float>(cornerDiff.magnitude(), static_cast<float>(info.minBezierDistance));
		MTLog::instance()->log("\tsize: " + std::to_string(cornerDiff.magnitude()) + "; minDistance: " + std::to_string(minDistance) + "\n");

		// first point (on surface of bounding box)
		int p0Side = -1;
		const cc::Vec3f p0 = rnd.pointOnBbox(boundingBox, p0Side, -1);
		bezierPoints.push_back(p0);
		MTLog::instance()->log("\tp0: face " + std::to_string(p0Side) + " at (" + std::to_string(p0.x) + ", " + std::to_string(p0.y) + ", " + std::to_string(p0.z) + ")\n");

		// second and third are randomly generated
		bezierPoints.push_back(rnd.pointInBBox(boundingBox));
		MTLog::instance()->log("\tp1: (" + std::to_string(bezierPoints.back().x) + ", " + std::to_string(bezierPoints.back().y) + ", " + std::to_string(bezierPoints.back().z) + ")\n");
		bezierPoints.push_back(rnd.pointInBBox(boundingBox));
		MTLog::instance()->log("\tp2: (" + std::to_string(bezierPoints.back().x) + ", " + std::to_string(bezierPoints.back().y) + ", " + std::to_string(bezierPoints.back().z) + ")\n");

		// fourth is on another side and must be a certain percentage of the size of the bounding box away from the first point
		const unsigned int MAX_ITERATIONS = 10;
		unsigned int iteration = 0;
		int p3Side = -1;
		cc::Vec3f p3;
		do {
			p3 = rnd.pointOnBbox(boundingBox, p3Side, p0Side);
			MTLog::instance()->log("\t" + std::string(p3.distance(p0) < minDistance ? "FAILED" : "SUCCEEDED") + " with distance " + std::to_string(p3.distance(p0)) + "\n");
			if( iteration++ >= MAX_ITERATIONS ) { // increment and check
				MTLog::instance()->log("\tMaximum iterations reached (" + std::to_string(MAX_ITERATIONS) + "), accepting last value.\n");
				break;
			}
			
		} while(p3.distance(p0) < minDistance);
		bezierPoints.push_back(p3);
		MTLog::instance()->log("\tp3: face " + std::to_string(p3Side) + " at (" + std::to_string(p3.x) + ", " + std::to_string(p3.y) + ", " + std::to_string(p3.z) + "\n");
	}

	// extract points along bezier curve
	BezierPath bezier;
	bezier.setControlPoints(bezierPoints);
	//for( auto& pnt : bezier.getDrawingPoints(info.samples) ) {
	for( auto& pnt : bezier.getDrawingPointsAlternate(info.samples) ) {
		outPoints.push_back(pnt);
	}

	// fluctuate points from curve to break how uniform they appear
	if( !cc::math::equal<double>(info.flux, 0.0) ) {
		const cc::Vec3f cornerDiff = boundingBox.getCorner(BoundingBox::Corner::BottomLeftBack) - boundingBox.getCorner(BoundingBox::Corner::TopRightFront);
		const float fluxAmount = cc::math::percent<float>(cornerDiff.magnitude(), static_cast<float>(info.flux));
		for( auto& pnt : outPoints ) {
			pnt += cc::Vec3f(rnd.nextReal(-fluxAmount, fluxAmount), rnd.nextReal(-fluxAmount, fluxAmount), rnd.nextReal(-fluxAmount, fluxAmount));
		}
	}

	// add some uniformly random points to add some extra detail away from the curve
	for( unsigned int i = 0; i < info.uniformCount; ++i ) {
		outPoints.push_back(rnd.pointInBBox(boundingBox));
	}

	fflush(stdout);
}