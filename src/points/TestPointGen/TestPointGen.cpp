#include "TestPointGen.hpp"
#include <random>
#include <BezierPath.hpp>
#include <Random.hpp>
#include <Log.hpp>

TestPointGen::TestPointGen()
	: IPointGen() {
}

TestPointGen::~TestPointGen() {
}

void TestPointGen::setUserPoints( const std::vector<cc::Vec3f>& userPoints ) {
}

void TestPointGen::generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) {
	const BoundingBox bbox = sourceModel.computeBoundingBox();
	const float minx = bbox.minX();
	const float miny = bbox.minY();
	const float minz = bbox.minZ();
	const float maxx = bbox.maxX();
	const float maxy = bbox.maxY();
	const float maxz = bbox.maxZ();

	// bezier stuff
	std::vector<cc::Vec3f> controlPoints;
	int firstSide = -1;
	const cc::Vec3f firstPoint = randomPointOnUnitSide(bbox, -1, firstSide, nullptr);
	controlPoints.push_back(firstPoint); // p0 - begin
	//controlPoints.push_back(bbox.getCenter() + bbox.getHalfExtents()); // p0 - begin
	controlPoints.push_back(randomPointInBounds(minx, maxx, miny, maxy, minz, maxz)); // p1
	controlPoints.push_back(randomPointInBounds(minx, maxx, miny, maxy, minz, maxz)); // p2
	int lastSide = -1;
	const cc::Vec3f lastPoint = randomPointOnUnitSide(bbox, firstSide, lastSide, &firstPoint);
	controlPoints.push_back(lastPoint); // p3 - end
	//controlPoints.push_back(bbox.getCenter() - bbox.getHalfExtents()); // p3 - end
	BezierPath bezier;
	bezier.setControlPoints(controlPoints);
	for( auto& pnt : bezier.getDrawingPoints() ) {
		outPoints.push_back(pnt);
	}
	for( unsigned int i = 0; i < 10; ++i ) {
		outPoints.push_back(randomPointInBounds(minx, maxx, miny, maxy, minz, maxz));
	}
	for( unsigned int i = 0; i < outPoints.size(); ++i ) {
		const float FLUX = 0.05f;
		outPoints[i] += cc::Vec3f(cc::math::randRange<float>(-FLUX, FLUX), cc::math::randRange<float>(-FLUX, FLUX), cc::math::randRange<float>(-FLUX, FLUX));
	}

	printf("First point:\n");
	printf("\tside: %d\n", firstSide);
	printf("\tpos: %f, %f, %f\n", firstPoint.x, firstPoint.y, firstPoint.z);
	printf("Second point:\n");
	printf("\tside: %d\n", lastSide);
	printf("\tpos: %f, %f, %f\n", lastPoint.x, lastPoint.y, lastPoint.z);
	fflush(stdout);



#if 0
	// terrible sampling using bbox and poisson
	std::random_device rnd;
	std::mt19937 gen(rnd());
	std::poisson_distribution<> d(4);
	const float step = 0.25f;
	for( float xx = minx; xx < maxx; xx += step ) {
		for( float yy = miny; yy < maxy; yy += step ) {
			for( float zz = minz; zz < maxz; zz += step ) {
				const int num = d(gen);
				if( num >= 3 && num <= 5 ) {
					outPoints.push_back(cc::Vec3f(xx, yy, zz));
				}
			}
		}
	}
#endif

	//// random distribution based on importance points (weighted)
	//// http://en.cppreference.com/w/cpp/numeric/random/piecewise_constant_distribution
	//std::vector<cc::Vec3f> importancePoints;
	//importancePoints.push_back(bbox.getCorner(BoundingBox::Corner::TopLeftBack));
	//importancePoints.push_back(bbox.getCorner(BoundingBox::Corner::TopLeftFront));
	//importancePoints.push_back(bbox.getCorner(BoundingBox::Corner::TopRightBack));
	//importancePoints.push_back(bbox.getCorner(BoundingBox::Corner::TopRightFront));
	//importancePoints.push_back(bbox.getCorner(BoundingBox::Corner::BottomLeftBack));
	//importancePoints.push_back(bbox.getCorner(BoundingBox::Corner::BottomLeftFront));
	//importancePoints.push_back(bbox.getCorner(BoundingBox::Corner::BottomRightBack));
	//importancePoints.push_back(bbox.getCorner(BoundingBox::Corner::BottomRightFront));
	//importancePoints.push_back(bbox.getCenter());
	//std::vector<float> weights;
	//std::vector<float> interval_x;
	//std::vector<float> interval_y;
	//std::vector<float> interval_z;
	//for( unsigned int i = 0; i < 9; ++i ) {
	//	const cc::Vec3f& pnt = importancePoints[i];
	//	interval_x.push_back(pnt.x);
	//	interval_y.push_back(pnt.y);
	//	interval_z.push_back(pnt.z);
	//	weights.push_back(0.05f);
	//}
	//importancePoints.push_back(bbox.getCenter() + cc::Vec3f(bbox.getHalfExtents().x*0.5f, 0.0f, bbox.getHalfExtents().z*0.98f));
	//for( unsigned int i = 9; i < importancePoints.size(); ++i ) {
	//	const cc::Vec3f& pnt = importancePoints[i];
	//	interval_x.push_back(pnt.x);
	//	interval_y.push_back(pnt.y);
	//	interval_z.push_back(pnt.z);
	//	weights.push_back(0.55f);
	//}
	//std::piecewise_linear_distribution<float> dx(interval_x.begin(), interval_x.end(), weights.begin());
	//std::piecewise_linear_distribution<float> dy(interval_y.begin(), interval_y.end(), weights.begin());
	//std::piecewise_linear_distribution<float> dz(interval_z.begin(), interval_z.end(), weights.begin());
	//std::random_device rnd;
	//std::mt19937 gen(rnd());
	//for( unsigned int i = 0; i < pointCount; ++i ) {
	//	const float x = dx(gen);
	//	const float y = dy(gen);
	//	const float z = dz(gen);
	//	outPoints.push_back(cc::Vec3f(x, y, z));
	//}

	//// random points in a cube
	//for( unsigned int i = 0; i < pointCount; ++i ) {
	//	const float x = minx + (float(rand()) / RAND_MAX) * (maxx - minx);
	//	const float y = miny + (float(rand()) / RAND_MAX) * (maxy - miny);
	//	const float z = minz + (float(rand()) / RAND_MAX) * (maxz - minz);
	//	outPoints.push_back(cc::Vec3f(x, y, z));
	//}
}

cc::Vec3f TestPointGen::randomPointInBounds( float minx, float maxx, float miny, float maxy, float minz, float maxz ) const {
	const float x = minx + (float(rand()) / RAND_MAX) * (maxx - minx);
	const float y = miny + (float(rand()) / RAND_MAX) * (maxy - miny);
	const float z = minz + (float(rand()) / RAND_MAX) * (maxz - minz);
	return cc::Vec3f(x, y, z);
}

cc::Vec3f TestPointGen::randomPointOnUnitSide( const BoundingBox& bb, int notOnThisSide, int& outChosenSide, const cc::Vec3f* lastPoint ) const {
	Random<float, int> rnd;

	// pick a side that's not been picked before
	int newSide = notOnThisSide;
	do {
		newSide = rnd.nextInt(0, 5);
	} while(newSide == notOnThisSide);

	// update output chosen side
	outChosenSide = newSide;

	// generate point based on picked side
	cc::Vec3f pos = bb.getCenter();
	const cc::Vec3f& half = bb.getHalfExtents();

	// todo: move the looping out of this function
	// todo: add a max number of iterations and fail if it fails

	const cc::Vec3f cornerDiff = bb.getCorner(BoundingBox::Corner::BottomLeftBack) - bb.getCorner(BoundingBox::Corner::TopRightFront);
	const float minDistance = cc::math::percent<float>(cornerDiff.magnitude(), 50.0f);
	printf("MINIMUM DISTANCE: mindist: %f\n", minDistance);

	do {
		pos = bb.getCenter();
		switch( newSide ) {
			case 0: { // +x
					pos.x += half.x;
					pos.y += rnd.nextReal(-half.y, half.y);
					pos.z += rnd.nextReal(-half.z, half.z);
				break;
			}
			case 1: { // -x
					pos.x -= half.x;
					pos.y += rnd.nextReal(-half.y, half.y);
					pos.z += rnd.nextReal(-half.z, half.z);
				break;
			}
			case 2: { // +y
					pos.y += half.y;
					pos.x += rnd.nextReal(-half.x, half.x);
					pos.z += rnd.nextReal(-half.z, half.z);
				break;
			}
			case 3: { // -y
					pos.y -= half.y;
					pos.x += rnd.nextReal(-half.x, half.x);
					pos.z += rnd.nextReal(-half.z, half.z);
				break;
			}
			case 4: { // +z
					pos.z += half.z;
					pos.x += rnd.nextReal(-half.x, half.x);
					pos.y += rnd.nextReal(-half.y, half.y);
				break;
			}
			case 5: { // -z
					pos.z -= half.z;
					pos.x += rnd.nextReal(-half.x, half.x);
					pos.y += rnd.nextReal(-half.y, half.y);
				break;
			}
		}

		printf("Generated point with distance: %f\n", lastPoint != nullptr ? lastPoint->distance(pos) : 0.0f);
		fflush(stdout);

	} while(lastPoint != nullptr && lastPoint->distance(pos) < minDistance);


	return pos;
}