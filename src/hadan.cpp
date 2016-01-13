/**
 * hadan
 *
 * author:
 *    daniel green
 *
 * usage:
 *    hadan <mesh_name>
 *    <mesh_name> is a string representing the path of the mesh to fracture
 */


#include <maya/MIOStream.h>
#include <maya/MSimple.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include "Model.hpp"
#include "ClipMesh.hpp"
#include "MayaHelper.hpp"

DeclareSimpleCommand(hadan, "KasumiL5x", "0.0.1-dev");

MStatus hadan::doIt( const MArgList& args ) {
	if( args.length() < 1 ) {
		displayError("Hadan ERROR: Incorrect number of arguments.");
		return MS::kFailure; // must have right number of arguments
	}

	MDagPath inputObjectPath;
	if( !MayaHelper::getObjectFromString(args.asString(0).asChar(), inputObjectPath) ) {
		displayError("Hadan ERROR: Object not found.");
		return MS::kFailure; // object must exist
	}

	if( !MayaHelper::hasMesh(inputObjectPath) ) {
		displayError("Hadan ERROR: Object is not a mesh.");
		return MS::kFailure; // object must be a mesh
	}

	MFnMesh mayaMesh(inputObjectPath);
	if( MayaHelper::doesMeshHaveHoles(mayaMesh) ) {
		displayError("Hadan ERROR: Mesh must be a closed convex polyhedron (no holes; welded closed)."); // object must be closed
		return MS::kFailure;
	}

	if( !MayaHelper::isMeshFullyClosed(inputObjectPath) ) {
		displayError("Hadan ERROR: Mesh is not closed.  An edge somewhere only has a single face.");
		return MS::kFailure;
	}

	Model fromMaya;
	MayaHelper::copyMFnMeshToModel(mayaMesh, fromMaya);
	fromMaya.buildExtendedData();
	
	ClipMesh cm(fromMaya);
	ClipMesh::Result clipResult = cm.clip(Plane(cc::Vec3f(0.0f, 0.906307817f, -0.422618270f), 0.226576954f));
	if( ClipMesh::Result::Invisibubble == clipResult || ClipMesh::Result::Visible == clipResult ) {
		// if the mesh is invisible, it isn't rendered.
		// if the mesh is visible, it hasn't changed.
		return MS::kSuccess;
	}

	Model clippedModel;
	if( !cm.convert(&clippedModel) ) {
		displayError("Hadan ERROR: Failed to convert from ClipMesh to application Model.");
		return MS::kFailure;
	}

	MFnMesh outMayaMesh;
	MayaHelper::copyModelToMFnMesh(clippedModel, outMayaMesh);

	return MStatus::kSuccess;
}