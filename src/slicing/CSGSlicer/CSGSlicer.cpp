#include "CSGSlicer.hpp"
#include <maya/MItMeshVertex.h>
#include <maya/MIntArray.h>
#include <maya/MPointArray.h>
#include "../../ConvexTriangulator.hpp"
#include "../../MayaHelper.hpp"
#include <mutex>

#define CSGJS_HEADER_ONLY
#include "slicing/CSGSlicer/csgjs.cpp"

static std::mutex CSGJS_CREATEMESH_MUTEX;

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

	// convert cell into csgjs_model
	csgjs_model cellModel;

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
	{
		std::unique_lock<std::mutex> lock(CSGJS_CREATEMESH_MUTEX);
		outMesh.create(pointArray.length(), faceCounts.length(), pointArray, faceCounts, faceConnects);
	}

	return true;
}