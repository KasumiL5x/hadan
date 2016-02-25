#include "ClosedConvexSlicer.hpp"
#include "ClipMesh.hpp"
#include "../../MayaHelper.hpp"

ClosedConvexSlicer::ClosedConvexSlicer()
	: IMeshSlicer() {
}

ClosedConvexSlicer::~ClosedConvexSlicer() {
}

bool ClosedConvexSlicer::setSource( MFnMesh& source ) {
	MayaHelper::copyMFnMeshToModel(source.dagPath(), _inputModel);
	_inputModel.buildExtendedData();
	return (_inputModel.getVertices().size() != 0 && _inputModel.getIndices().size() != 0);
}

bool ClosedConvexSlicer::slice( const Cell& cell, MFnMesh& outMesh ) {
	ClipMesh clipMesh(_inputModel);

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
		Model outModel;
		if( !clipMesh.convert(&outModel) ) {
			return false;
		}
		MayaHelper::copyModelToMFnMesh(outModel, outMesh);
		return true;
	} else {
		return false;
	}
}