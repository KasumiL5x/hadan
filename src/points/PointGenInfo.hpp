#ifndef __point_gen_info__
#define __point_gen_info__

#include <vector>
#include <cc/Vec3.hpp>

struct PointGenInfo {
	// amount of uniformly scattered points to generate
	unsigned int uniformCount;
	// primary number of points
	unsigned int primaryCount;
	// secondary number of points
	unsigned int secondaryCount;
	// fluctuation percentage
	double flux;
	// user-provided source points
	std::vector<cc::Vec3f> userPoints;

	PointGenInfo() {
		uniformCount = 0;
		primaryCount = 0;
		secondaryCount = 0;
		flux = 0.0;
		userPoints = std::vector<cc::Vec3f>();
	}
};

#endif /* __point_gen_info__ */