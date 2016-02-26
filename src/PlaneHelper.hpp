#ifndef __plane_helper__
#define __plane_helper__

#include <vector>
#include "Plane.hpp"
#include "Model.hpp"

namespace PlaneHelper {
	static bool planeFromPoints( const std::vector<cc::Vec3f>& points, Plane& outPlane ) {
		const unsigned int n = static_cast<unsigned int>(points.size());
		// needs at least 3 points
		if( n < 3 ) {
			return false;
		}

		//float residualError;
		//Plane::fitPointsToPlane(points, residualError, outPlane);
		//outPlane.normal = -outPlane.normal;
		//outPlane.constant = -outPlane.constant;

		cc::Vec3f sum(0.0f, 0.0f, 0.0f);
		for( unsigned int i = 0; i < 3; ++i ) {
			sum += points[i];
		}			
		const cc::Vec3f centroid = sum * (1.0f / 3.0f);
		const cc::Vec3f ab = points[1] - points[0]; // ba
		const cc::Vec3f ac = points[2] - points[0]; // ca
		//const cc::Vec3f otherNormal = ab.cross(ac).normalized();
		//const float otherConstant = centroid.dot(outPlane.normal);
		outPlane.normal = ab.cross(ac).normalized();
		outPlane.constant = -centroid.dot(outPlane.normal);



		//const cc::Vec3f ab = points[2] - points[0];
		//const cc::Vec3f ac = points[1] - points[0];
		//outPlane.normal = ab.cross(ac).normalized();
		//outPlane.constant = -points[0].dot(outPlane.normal);

		//cc::Vec3f sum(0.0f, 0.0f, 0.0f);
		//for( auto& p : points ) {
		//	sum += p;
		//}
		//const cc::Vec3f centroid = sum * (1.0f / static_cast<float>(n));

		//float xx = 0.0f; float xy = 0.0f; float xz = 0.0f;
		//float yy = 0.0f; float yz = 0.0f; float zz = 0.0f;
		//for( auto& p : points ) {
		//	const cc::Vec3f r = p - centroid;
		//	xx += r.x * r.x;
		//	xy += r.x * r.y;
		//	xz += r.x * r.z;
		//	yy += r.y * r.y;
		//	yz += r.y * r.z;
		//	zz += r.z * r.z;
		//}

		//const float det_x = yy*zz - yz*yz;
		//const float det_y = xx*zz - xz*xz;
		//const float det_z = xx*yy - xy*xy;
		//const float det_max = cc::math::maximum<float>(det_x, cc::math::maximum<float>(det_y, det_z));
		//// points do not span a plane
		//if( det_max <= 0.0f ) {
		//	return false;
		//}

		//cc::Vec3f dir(0.0f, 0.0f, 0.0f);
		//if( cc::math::equal<float>(det_max, det_x) ) {
		//	const float a = (xz*yz - xy*zz) / det_x;
		//	const float b = (xy*yz - xz*yy) / det_x;
		//	dir = cc::Vec3f(1.0f, a, b);
		//} else if( cc::math::equal<float>(det_max, det_y) ) {
		//	const float a = (yz*xz - xy*zz) / det_y;
		//	const float b = (xy*xz - yz*xx) / det_y;
		//	dir = cc::Vec3f(a, 1.0f, b);
		//} else {
		//	const float a = (yz*xy - xz*yy) / det_z;
		//	const float b = (xz*xy - yz*xx) / det_z;
		//	dir = cc::Vec3f(a, b, 1.0f);
		//}

		//outPlane.normal = dir.normalized();
		//outPlane.constant = -centroid.dot(outPlane.normal);
		return true;
	}

	static void planeToModel( const Plane& plane, float size, const cc::Vec3f& position, Model& outModel ) {
		cc::Vec3f right = plane.normal.cross(cc::Vec3f::up()).normalized();
		if( 0.0f == right.magnitude() ){ 
			right = cc::Vec3f::right();
		}
		const cc::Vec3f surface = right.cross(plane.normal).normalized();
		const cc::Vec3f origin = cc::Vec3f(0.0f, plane.constant, 0.0f);
		
		// top left
		const cc::Vec3f p0 = (right * -size) + (surface * size);
		// top right
		const cc::Vec3f p1 = (right * size) + (surface * size);
		// bottom right
		const cc::Vec3f p2 = (right * size) + (surface * -size);
		// bottom left
		const cc::Vec3f p3 = (right * -size) + (surface * -size);

		outModel.addVertex(Vertex(position + (plane.normal * plane.constant) + p0));
		outModel.addVertex(Vertex(position + (plane.normal * plane.constant) + p1));
		outModel.addVertex(Vertex(position + (plane.normal * plane.constant) + p2));
		outModel.addVertex(Vertex(position + (plane.normal * plane.constant) + p3));

		//printf("position: %f, %f, %f\n", position.x, position.y, position.z);

		//outModel.addVertex(Vertex(position + plane.normal * -plane.constant + p0));
		//outModel.addVertex(Vertex(position + plane.normal * -plane.constant + p1));
		//outModel.addVertex(Vertex(position + plane.normal * -plane.constant + p2));
		//outModel.addVertex(Vertex(position + plane.normal * -plane.constant + p3));

		outModel.addIndex(0);
		outModel.addIndex(3);
		outModel.addIndex(1);
		outModel.addIndex(3);
		outModel.addIndex(2);
		outModel.addIndex(1);

	}
} // PlaneHelper

#endif /* __plane_helper__ */