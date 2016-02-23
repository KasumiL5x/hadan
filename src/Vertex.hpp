#ifndef __vertex__
#define __vertex__

#include <cc/Vec3.hpp>
#include <cc/Vec2.hpp>

struct Vertex {
	cc::Vec3f position;
	cc::Vec2f texcoord;

	Vertex()
		: position(0.0f, 0.0f, 0.0f), texcoord(0.0f, 0.0f) {
	}

	Vertex( const cc::Vec3f& inPosition )
		: position(inPosition) {
	}
	Vertex( const cc::Vec3f& inPosition, const cc::Vec2f& inTexcoord )
		: position(inPosition), texcoord(inTexcoord) {
	}
};

#endif /* __vertex__ */