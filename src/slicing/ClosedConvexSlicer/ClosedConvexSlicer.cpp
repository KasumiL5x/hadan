#include "ClosedConvexSlicer.hpp"
#include <slicing/ClipMesh.hpp>

ClosedConvexSlicer::ClosedConvexSlicer()
	: IMeshSlicer() {
}

ClosedConvexSlicer::~ClosedConvexSlicer() {
}

bool ClosedConvexSlicer::slice( Model& sourceModel, const Cell& cell, Model& outModel ) const {
	// convert the source mesh into a ClipMesh
	ClipMesh clipMesh(sourceModel);
	// cut the ClipMesh with all planes of the cell
	bool anyResult = false;
	for( unsigned int i = 0; i < cell.getPlaneCount(); ++i ) {
		const Plane& currPlane = cell.getPlanes()[i];
		if( ClipMesh::Result::Dissected == clipMesh.clip(currPlane) ) {
			anyResult = true;
		}
	}
	// if any cuts were successful, return converted model
	if( anyResult ) {
		return clipMesh.convert(&outModel);
	} else {
		return false;
	}
}