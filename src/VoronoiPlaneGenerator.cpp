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
	voro::voronoicell cell;
	if( cla.start() ) {
		do {
			if( container.compute_cell(cell, cla) ) {
				// compute offset for current cell
				const double* pp = container.p[cla.ijk] + container.ps * cla.q;
				const double off_x = *pp;
				const double off_y = pp[1];
				const double off_z = pp[2];

				 // get all cell points in a usable format
				std::vector<cc::Vec3f> cellPoints;
				for( int i = 0; i < cell.p; ++i ) {
					const float x = static_cast<float>(off_x + (0.5 * cell.pts[3*i]));
					const float y = static_cast<float>(off_y + (0.5 * cell.pts[3*i+1]));
					const float z = static_cast<float>(off_z + (0.5 * cell.pts[3*i+2]));
					cellPoints.push_back(cc::Vec3f(x, y, z));
				}

				// extract planes from points
				std::vector<int> faceVertices;
				cell.face_vertices(faceVertices);
				int faceCounter = 0;
				for( int i = 0;  i < faceVertices.size(); ) {
					const int vertsInFace = faceVertices[i]; // k, k+k1, k+k1+k2, ...
					std::vector<cc::Vec3f> facePoints;
					for( int j = i+1; j < i+vertsInFace+1; ++j ) {
						const int vtxIdx = faceVertices[j];
						facePoints.push_back(cellPoints[vtxIdx]);
					}

					Plane plane;
					if( PlaneHelper::planeFromPoints(facePoints, plane) ) {
						outPlanes.push_back(plane);
					}

					i += vertsInFace+1;
					faceCounter += 1;
				}

				outPlaneCounts.push_back(faceCounter);
			}
		} while(cla.inc());
	}

	container.draw_cells_gnuplot("C:/Users/daniel/Desktop/cells.gnu");
	container.draw_particles("C:/Users/daniel/Desktop/particles.gnu");
}