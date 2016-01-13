#include "VoronoiPlaneGenerator.hpp"
#include <voro++.hh>
#include "PlaneHelper.hpp"

VoronoiPlaneGenerator::VoronoiPlaneGenerator()
	: IPlaneGenerator() {
}

VoronoiPlaneGenerator::~VoronoiPlaneGenerator() {
}

void VoronoiPlaneGenerator::generatePlanes( const BoundingBox& bbox, const std::vector<cc::Vec3f>& samplePoints, std::vector<Plane>& outPlanes, std::vector<int>& outPlaneCounts ) const {
	// create container
	const int initMem = 8;
	const int resx = 6;
	const int resy = 6;
	const int resz = 6;
	voro::container container(bbox.minX(), bbox.maxX(), bbox.minY(), bbox.maxY(), bbox.minZ(), bbox.maxZ(), resx, resy, resz, false, false, false, initMem);

	// insert sample points
	for( unsigned int i = 0; i < static_cast<unsigned int>(samplePoints.size()); ++i ) {
		const cc::Vec3f& pos = samplePoints[i];
		container.put(i, static_cast<double>(pos.x), static_cast<double>(pos.y), static_cast<double>(pos.z));
	}

	voro::c_loop_all cla(container);
	if( cla.start() ) {
		do {
			voro::voronoicell cell;
			if( container.compute_cell(cell, cla) ) {
				// get vertex positions
				std::vector<double> vertices;
				cell.vertices(vertices);
				// get vertex indices
				std::vector<int> faceVertices;
				cell.face_vertices(faceVertices);

				// i think i'm parsing the vertices wrong?  the planes generated (and points?) seem off...
				// check the docs (lol) for anything useful
				giorgjiorjijirt

				unsigned int faceCounter = 0;
				for( unsigned int i = 0; i < static_cast<unsigned int>(faceVertices.size()); ) {
					const int vertsInFace = faceVertices[i]; // k, k+k1, k+k1+k2, ...
					std::vector<cc::Vec3f> ccPoints;
					for( int j = i+1; j < i+vertsInFace+1; ++j ) {
						const int vtxIdx = faceVertices[j];
						ccPoints.push_back(cc::Vec3f(vertices[vtxIdx], vertices[vtxIdx+1], vertices[vtxIdx+2]));
					}

					Plane plane;
					if( PlaneHelper::planeFromPoints(ccPoints, plane) ) {
						outPlanes.push_back(plane);
					}

					i += vertsInFace+1;
					faceCounter += 1;
				}

				outPlaneCounts.push_back(faceCounter);
			}
		} while(cla.inc());
	}

	container.draw_particles("C:/Users/daniel/Desktop/particles.gnu");
	container.draw_cells_gnuplot("C:/Users/daniel/Desktop/cells.gnu");
}