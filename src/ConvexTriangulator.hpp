#ifndef __convex_triangulator__
#define __convex_triangulator__

#include <vector>
#include <stack>
#include <array>
#include <cc/Vec3.hpp>

static std::vector<std::array<int, 3>> HadanConvexTriangulate( const std::vector<cc::Vec3f>& points ) {
	std::vector<std::array<int, 3>> indices;

	if( points.size () < 3 ) {
		return indices;
	}

	struct StackVertex {
		cc::Vec3f point;
		int index;
	};

	std::stack<StackVertex> stack;
	int indexCounter = 0;
	for( const auto& p : points ) {
		StackVertex sv;
		sv.point = p;
		sv.index = indexCounter++;
		stack.push(sv);
	}

	const StackVertex p0 = stack.top();
	stack.pop();

	StackVertex pHelper = stack.top();
	stack.pop();

	while( !stack.empty() ) {
		const StackVertex pTemp = stack.top();
		stack.pop();

		indices.push_back({p0.index, pHelper.index, pTemp.index});

		pHelper = pTemp;
	}

	return indices;
}

#endif /* __convex_triangulator__ */