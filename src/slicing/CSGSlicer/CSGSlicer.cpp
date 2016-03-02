#include "CSGSlicer.hpp"
#include <maya/MItMeshVertex.h>
#include <maya/MIntArray.h>
#include <maya/MPointArray.h>
#include "../../ConvexTriangulator.hpp"

#define CSGJS_HEADER_ONLY
#include "csgjs.cpp"

CSGSlicer::CSGSlicer()
	: IMeshSlicer() {
}

CSGSlicer::~CSGSlicer() {
}

bool CSGSlicer::setSource( MFnMesh& source ) {
	_sourceMesh = source.dagPath();
	return true;
}

bool CSGSlicer::slice( const Cell& cell, const MeshSlicerInfo& info, MFnMesh& outMesh ) {
	// copy maya mesh to csg.js model
	csgjs_model jsModel;
	MItMeshVertex it(_sourceMesh);
	while( !it.isDone() ) {
		csgjs_vertex vtx;
		// get position
		MPoint pos = it.position(MSpace::kWorld);
		vtx.pos = csgjs_vector(static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z));
		// add position
		jsModel.vertices.push_back(vtx);
		it.next();
	}
	MIntArray triangleCounts;
	MIntArray triangleVertices;
	MFnMesh(_sourceMesh).getTriangles(triangleCounts, triangleVertices);
	for( unsigned int i = 0; i < triangleVertices.length(); ++i ) {
		jsModel.indices.push_back(triangleVertices[i]);
	}
	//for( unsigned int i = 0; i < triangleVertices.length()/3; ++i ) {
	//	jsModel.counts.push_back(3); // getting triangles so always 3s
	//}

	// convert cell into csgjs_model
	csgjs_model cellModel;
	//cellModel.vertices.push_back(csgjs_vertex(csgjs_vector(-0.5f, -0.5f,  0.5f)));
	//cellModel.vertices.push_back(csgjs_vertex(csgjs_vector( 0.5f, -0.5f,  0.5f)));
	//cellModel.vertices.push_back(csgjs_vertex(csgjs_vector( 0.5f,  0.5f,  0.5f)));
	//cellModel.vertices.push_back(csgjs_vertex(csgjs_vector(-0.5f,  0.5f,  0.5f)));
	//cellModel.vertices.push_back(csgjs_vertex(csgjs_vector(-0.5f, -0.5f, -0.5f)));
	//cellModel.vertices.push_back(csgjs_vertex(csgjs_vector( 0.5f, -0.5f, -0.5f)));
	//cellModel.vertices.push_back(csgjs_vertex(csgjs_vector( 0.5f,  0.5f, -0.5f)));
	//cellModel.vertices.push_back(csgjs_vertex(csgjs_vector(-0.5f,  0.5f, -0.5f)));
	//cellModel.indices.push_back(0); cellModel.indices.push_back(1); cellModel.indices.push_back(2); // front
	//cellModel.indices.push_back(2); cellModel.indices.push_back(3); cellModel.indices.push_back(0);
	//cellModel.indices.push_back(1); cellModel.indices.push_back(5); cellModel.indices.push_back(6); // top
	//cellModel.indices.push_back(6); cellModel.indices.push_back(2); cellModel.indices.push_back(1);
	//cellModel.indices.push_back(7); cellModel.indices.push_back(6); cellModel.indices.push_back(5); // back
	//cellModel.indices.push_back(5); cellModel.indices.push_back(4); cellModel.indices.push_back(7);
	//cellModel.indices.push_back(4); cellModel.indices.push_back(0); cellModel.indices.push_back(3); // bottom
	//cellModel.indices.push_back(3); cellModel.indices.push_back(7); cellModel.indices.push_back(4);
	//cellModel.indices.push_back(4); cellModel.indices.push_back(5); cellModel.indices.push_back(1); // left
	//cellModel.indices.push_back(1); cellModel.indices.push_back(0); cellModel.indices.push_back(4);
	//cellModel.indices.push_back(3); cellModel.indices.push_back(2); cellModel.indices.push_back(6); // right
	//cellModel.indices.push_back(6); cellModel.indices.push_back(7); cellModel.indices.push_back(3);
	//for( int i = 0; i < 12; ++i ) {
	//	cellModel.counts.push_back(3);
	//}

	// TODO: I need to triangulate the cell's points somehow (or find a another library that triangulates the cells
	//       automatically).  I also need to revert my changes to csg.js, as it is only suitable for triangle meshes,
	//       and my changes assume that is not the case (a.k.a remove extended stuff).  Look online for the above.
	//       As a test, I created a triangulated cube above and it worked fine (although it was VERY slow).
	//gdjoigrjoipgrjoigjiorijogr
	//unsigned int offset = 0;
	//for( const auto& pnt : cell.getPoints() ) {
	//	csgjs_vertex vtx;
	//	vtx.pos = csgjs_vector(pnt.x, pnt.y, pnt.z);
	//	cellModel.vertices.push_back(vtx);
	//}
	//for( const auto& count : cell.getCounts() ) {
	//	cellModel.counts.push_back(count);
	//}
	//for( const auto& idx : cell.getIndices() ) {
	//	cellModel.indices.push_back(idx);
	//}

	int indexOffset = 0;
	const auto& planePoints = cell.getPlanePoints();
	for( const auto& points : planePoints ) {
		for( const auto& pnt : points ) {
			csgjs_vertex vtx;
			vtx.pos = csgjs_vector(pnt.x, pnt.y, pnt.z);
			cellModel.vertices.push_back(vtx);
		}
		const auto& indices = HadanConvexTriangulate(points);
		for( const auto& idx : indices ) {
			cellModel.indices.push_back(indexOffset + idx[0]);
			cellModel.indices.push_back(indexOffset + idx[1]);
			cellModel.indices.push_back(indexOffset + idx[2]);
		}
		//cellModel.counts.push_back(3);

		indexOffset += static_cast<int>(points.size());
	}

	// perform intersection
	const csgjs_model result = csgjs_intersection(jsModel, cellModel);

	// copy back to maya mesh
	MPointArray pointArray;
	for( const auto& vtx : result.vertices ) {
		pointArray.append(MPoint(vtx.pos.x, vtx.pos.y, vtx.pos.z));
	}
	MIntArray faceConnects;
	for( const auto& idx : result.indices ) {
		faceConnects.append(idx);
	}
	MIntArray faceCounts;
	for( size_t i = 0; i < result.indices.size() / 3; ++i ) {
		faceCounts.append(3);
	}
	//for( const auto& cnt : result.counts ) {
	//	faceCounts.append(cnt);
	//}
	outMesh.create(pointArray.length(), faceCounts.length(), pointArray, faceCounts, faceConnects);

	return true;
}