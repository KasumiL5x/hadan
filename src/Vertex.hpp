#ifndef __vertex__
#define __vertex__

#include <cc/Vec3.hpp>
#include <cc/Vec2.hpp>

struct Vertex {
	cc::Vec3f position;
	cc::Vec3f normal;
	cc::Vec2f texcoord;

	Vertex()
		: position(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), texcoord(0.0f, 0.0f) {
	}

	Vertex( const cc::Vec3f& inPosition )
		: position(inPosition), normal(0.0f, 0.0f, 0.0f), texcoord(0.0f, 0.0f) {
	}
	Vertex( const cc::Vec3f& inPosition, const cc::Vec3f& inNormal )
		: position(inPosition), normal(inNormal), texcoord(0.0f, 0.0f) {
	}
	Vertex( const cc::Vec3f& inPosition, const cc::Vec3f& inNormal, const cc::Vec2f& inTexcoord )
		: position(inPosition), normal(inNormal), texcoord(inTexcoord) {
	}
};

#endif /* __vertex__ */