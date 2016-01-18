#ifndef __points_utils__
#define __points_utils__

#include <cc/Vec3.hpp>
#include <BoundingBox.hpp>
#include <Random.hpp>

namespace PointsUtils {
	static cc::Vec3f randomPointInBbox( const BoundingBox& bb ) {
		Random<float, int> rnd;
		const float x = rnd.nextReal(bb.minX(), bb.maxX());
		const float y = rnd.nextReal(bb.minY(), bb.maxY());
		const float z = rnd.nextReal(bb.minZ(), bb.maxZ());
		return cc::Vec3f(x, y, z);
	}

	static cc::Vec3f randomPointOnBboxSide( const BoundingBox& bb, int& outChosenSide, int notThisSide=-1 ) {
		Random<float, int> rnd;

		// pick a side that's not been picked before
		int newSide = notThisSide;
		do {
			newSide = rnd.nextInt(0, 5);
		} while(newSide == notThisSide);

		// update chosen side output
		outChosenSide = newSide;

		// generate point based on picked side
		const cc::Vec3f& half = bb.getHalfExtents();
		cc::Vec3f pos = bb.getCenter();
		switch( newSide ) {
			case 0: { // +x
					pos.x += half.x;
					pos.y += rnd.nextReal(-half.y, half.y);
					pos.z += rnd.nextReal(-half.z, half.z);
				break;
			}
			case 1: { // -x
					pos.x -= half.x;
					pos.y += rnd.nextReal(-half.y, half.y);
					pos.z += rnd.nextReal(-half.z, half.z);
				break;
			}
			case 2: { // +y
					pos.y += half.y;
					pos.x += rnd.nextReal(-half.x, half.x);
					pos.z += rnd.nextReal(-half.z, half.z);
				break;
			}
			case 3: { // -y
					pos.y -= half.y;
					pos.x += rnd.nextReal(-half.x, half.x);
					pos.z += rnd.nextReal(-half.z, half.z);
				break;
			}
			case 4: { // +z
					pos.z += half.z;
					pos.x += rnd.nextReal(-half.x, half.x);
					pos.y += rnd.nextReal(-half.y, half.y);
				break;
			}
			case 5: { // -z
					pos.z -= half.z;
					pos.x += rnd.nextReal(-half.x, half.x);
					pos.y += rnd.nextReal(-half.y, half.y);
				break;
			}
		}
		return pos;
	}
}

#endif /* __points_utils__ */