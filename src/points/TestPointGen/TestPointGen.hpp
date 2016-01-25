#ifndef __testsamplepointgenerator__
#define __testsamplepointgenerator__

#include <points/IPointGen.hpp>

class TestPointGen : public IPointGen {
public:
	TestPointGen();
	~TestPointGen();

	virtual void generateSamplePoints( const Model& sourceModel, const PointGenInfo& info, std::vector<cc::Vec3f>& outPoints ) override;
};

#endif /* __testsamplepointgenerator__ */