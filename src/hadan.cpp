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

// todo:
//    - fix plane generation (look at voronoplanegenerator.cpp for a note)
//    - fix maya vertices not being in world space


#include <maya/MIOStream.h>
#include <maya/MSimple.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include "Model.hpp"
#include "ClipMesh.hpp"
#include "MayaHelper.hpp"
#include "TestSamplePointGenerator.hpp"
#include "VoronoiPlaneGenerator.hpp"
#include "PlaneHelper.hpp"

DeclareSimpleCommand(hadan, "KasumiL5x", "0.0.1-dev");

ISamplePointGenerator* pointGenerator = nullptr;
IPlaneGenerator* planeGenerator = nullptr;

ISamplePointGenerator* createSamplePointGenerator() {
	return new TestSamplePointGenerator();
}

IPlaneGenerator* createPlaneGenerator() {
	return new VoronoiPlaneGenerator();
}

void destroy() {
	if( planeGenerator != nullptr ) {
		delete planeGenerator;
		planeGenerator = nullptr;
	}
	if( pointGenerator != nullptr ) {
		delete pointGenerator;
		pointGenerator = nullptr;
	}
}

MStatus hadan::doIt( const MArgList& args ) {
	// 1. generate sample points
	// 2. generate cutting planes
	// 3. create cut geometry

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

	// copy maya mesh into custom model and build extended data
	Model fromMaya;
	MayaHelper::copyMFnMeshToModel(mayaMesh, fromMaya);
	fromMaya.buildExtendedData();

	// create a sample point generator and generate sample points
	pointGenerator = createSamplePointGenerator();
	std::vector<cc::Vec3f> samplePoints;
	pointGenerator->generateSamplePoints(fromMaya, 2, samplePoints);

	if( samplePoints.empty() ) {
		printf("Hadan ERROR: Not enough sample points were generated.\n");
		destroy();
		return MS::kFailure;
	}

	// create a plane generator and generate cutting planes
	planeGenerator = createPlaneGenerator();
	std::vector<Plane> outPlanes;
	std::vector<int> outPlaneCounts;
	planeGenerator->generatePlanes(fromMaya.computeBoundingBox(), samplePoints, outPlanes, outPlaneCounts);

	if( outPlanes.empty() || outPlaneCounts.empty() ) {
		printf("Hadan ERROR: Generated cutting planes were inadequate.\n");
		destroy();
		return MS::kFailure;
	}

	// cut out all cells creating a new piece of geometry for each
	int offset = 0;
	for( unsigned int i = 0; i < static_cast<unsigned int>(outPlaneCounts.size()); ++i ) {
		// copy the maya mesh for slicing
		ClipMesh cm(fromMaya);

		// cut with all planes for the cell
		bool anyResult = false;
		const int planeCount = outPlaneCounts[i];
		for( int j = 0; j < planeCount; ++j ) {
			const Plane& currPlane = outPlanes[offset++];
			if( ClipMesh::Result::Dissected == cm.clip(currPlane) ) {
				anyResult = true;
			}

			//
			// DEBUG
			//
			Model planeModel;
			PlaneHelper::planeToModel(currPlane, 1.0f, planeModel);
			MFnMesh planeMesh;
			MayaHelper::copyModelToMFnMesh(planeModel, planeMesh);
		}

		if( anyResult ) {
			Model clippedModel;
			if( cm.convert(&clippedModel) ) {
				MFnMesh outCellChunkMesh;
				MayaHelper::copyModelToMFnMesh(clippedModel, outCellChunkMesh);
			} else {
				printf("Hadan ERROR: Failed to convert clipped model.\n");
			}
		}
	}
	destroy();

	return MStatus::kSuccess;
}