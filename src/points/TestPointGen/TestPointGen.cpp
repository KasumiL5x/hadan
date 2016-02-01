#include "TestPointGen.hpp"

TestPointGen::TestPointGen()
	: IPointGen() {
}

TestPointGen::~TestPointGen() {
}

void TestPointGen::generateSamplePoints( const Model& sourceModel, const PointGenInfo& info, std::vector<cc::Vec3f>& outPoints ) {
	const BoundingBox bbox = sourceModel.computeBoundingBox();
}
