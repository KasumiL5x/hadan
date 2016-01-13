#ifndef __vertex__
#define __vertex__

#include <cc/Vec3.hpp>

struct Vertex {
	cc::Vec3f position;

	Vertex()
		: position(0.0f, 0.0f, 0.0f) {
	}

	Vertex( const cc::Vec3f& inPosition )
		: position(inPosition) {
	}
};

#endif /* __vertex__ */