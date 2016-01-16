#include "TestPointGen.hpp"

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

	// random points in a cube
	for( unsigned int i = 0; i < pointCount; ++i ) {
		const float x = minx + (float(rand()) / RAND_MAX) * (maxx - minx);
		const float y = miny + (float(rand()) / RAND_MAX) * (maxy - miny);
		const float z = minz + (float(rand()) / RAND_MAX) * (maxz - minz);
		outPoints.push_back(cc::Vec3f(x, y, z));
	}
}