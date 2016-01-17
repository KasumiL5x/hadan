#include "TestPointGen.hpp"
#include <random>
#include <BezierPath.hpp>

TestPointGen::TestPointGen()
	: IPointGen() {
}

TestPointGen::~TestPointGen() {
}



void TestPointGen::generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) const {
	const BoundingBox bbox = sourceModel.computeBoundingBox();
	const float minx = bbox.minX();
	const float miny = bbox.minY();
	const float minz = bbox.minZ();
	const float maxx = bbox.maxX();
	const float maxy = bbox.maxY();
	const float maxz = bbox.maxZ();

	// bezier stuff
	std::vector<cc::Vec3f> controlPoints;
	controlPoints.push_back(bbox.getCenter() + bbox.getHalfExtents()); // p0 - begin
	controlPoints.push_back(randomPointInBounds(minx, maxx, miny, maxy, minz, maxz)); // p1
	controlPoints.push_back(randomPointInBounds(minx, maxx, miny, maxy, minz, maxz)); // p2
	controlPoints.push_back(bbox.getCenter() - bbox.getHalfExtents()); // p3 - end
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