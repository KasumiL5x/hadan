#ifndef __random__
#define __random__

#include <random>
#include <cc/Vec3.hpp>
#include "BoundingBox.hpp"

template <class RealType, class IntType>
class Random {
public:
	Random() {
		std::random_device dev;
		_mt = std::mt19937(dev());
	}
	Random( unsigned int seed ) {
		_mt = std::mt19937(seed);
	}
	~Random() {
	}

	void setSeed( unsigned int seed ) {
		_mt = std::mt19937(seed);
	}

	RealType nextReal() {
		std::uniform_real_distribution<RealType> dist;
		return dist(_mt);
	}

	RealType nextReal( const RealType min, const RealType max ) {
		std::uniform_real_distribution<RealType> dist(min, max);
		return dist(_mt);
	}

	IntType nextInt() {
		std::uniform_int_distribution<IntType> dist;
		return dist(_mt);
	}

	IntType nextInt( const IntType min, const IntType max ) {
		std::uniform_int_distribution<IntType> dist(min, max);
		return dist(_mt);
	}

	cc::Vec3f pointInBBox( const BoundingBox& bb ) {
		const float x = nextReal(bb.minX(), bb.maxX());
		const float y = nextReal(bb.minY(), bb.maxY());
		const float z = nextReal(bb.minZ(), bb.maxZ());
		return cc::Vec3f(x, y, z);
	}

	// outChosenSide = output index for chosen side
	// notThisSide = input side to NOT pick
	// sides:
	//    0 = +x
	//    1 = -x
	//    2 = +y
	//    3 = -y
	//    4 = +z
	//    5 = -z
	cc::Vec3f pointOnBbox( const BoundingBox& bb, int& outChosenSide, int notThisSide=-1 ) {
		// pick a side that's not been picked before
		int newSide = notThisSide;
		do {
			newSide = nextInt(0, 5);
		} while(newSide == notThisSide);

		// update chosen side output
		outChosenSide = newSide;

		// generate point based on picked side
		const cc::Vec3f& half = bb.getHalfExtents();
		cc::Vec3f pos = bb.getCenter();
		switch( newSide ) {
			case 0: { // +x
					pos.x += half.x;
					pos.y += nextReal(-half.y, half.y);
					pos.z += nextReal(-half.z, half.z);
				break;
			}
			case 1: { // -x
					pos.x -= half.x;
					pos.y += nextReal(-half.y, half.y);
					pos.z += nextReal(-half.z, half.z);
				break;
			}
			case 2: { // +y
					pos.y += half.y;
					pos.x += nextReal(-half.x, half.x);
					pos.z += nextReal(-half.z, half.z);
				break;
			}
			case 3: { // -y
					pos.y -= half.y;
					pos.x += nextReal(-half.x, half.x);
					pos.z += nextReal(-half.z, half.z);
				break;
			}
			case 4: { // +z
					pos.z += half.z;
					pos.x += nextReal(-half.x, half.x);
					pos.y += nextReal(-half.y, half.y);
				break;
			}
			case 5: { // -z
					pos.z -= half.z;
					pos.x += nextReal(-half.x, half.x);
					pos.y += nextReal(-half.y, half.y);
				break;
			}
		}
		return pos;
	}

private:
	std::mt19937 _mt;
};

#endif /* __random__ */