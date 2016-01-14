/**
 * hadan
 *
 * author:
 *    daniel green
 *
 * usage:
 *    hadan <mesh_name> <slice_count>
 *    <mesh_name>   is a string representing the path of the mesh to fracture
 *    <slice_count> is the number of slices the source geometry should be split into (must be >=1)
 */

// todo:
//    - fix plane generation (look at voronoplanegenerator.cpp for a note)
//    - fix maya vertices not being in world space

#include <ctime>
#include <maya/MSimple.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MFnSet.h>
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
	srand(time(0));

	printf("Hadan beginning!\n");

	if( args.length() < 2 ) {
		displayError("Hadan ERROR: Incorrect number of arguments.");
		return MS::kFailure; // must have right number of arguments
	}

	// get and validate number of slices
	const int numSlices = args.asInt(1);
	if( numSlices < 1 ) {
		displayError("Hadan ERROR: Must have at least one slice!");
		return MS::kFailure;
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

	// debug: print bb info
	BoundingBox bb = fromMaya.computeBoundingBox();
	printf("minx(%f), maxx(%f), miny(%f), maxy(%f), minz(%f), maxz(%f)\n", bb.minX(), bb.maxX(), bb.minY(), bb.maxY(), bb.minZ(), bb.maxZ());

	// create a sample point generator and generate sample points
	pointGenerator = createSamplePointGenerator();
	std::vector<cc::Vec3f> samplePoints;
	pointGenerator->generateSamplePoints(fromMaya, static_cast<unsigned int>(numSlices), samplePoints);

	if( samplePoints.empty() ) {
		printf("Hadan ERROR: Not enough sample points were generated.\n");
		destroy();
		return MS::kFailure;
	}

	// create a plane generator and generate cutting planes
	planeGenerator = createPlaneGenerator();
	std::vector<Plane> outPlanes;
	std::vector<int> outPlaneCounts;
	std::vector<cc::Vec3f> outCellPositions;
	planeGenerator->generatePlanes(fromMaya.computeBoundingBox(), samplePoints, outPlanes, outPlaneCounts, outCellPositions);

	if( outPlanes.empty() || outPlaneCounts.empty() ) {
		printf("Hadan ERROR: Generated cutting planes were inadequate.\n");
		destroy();
		return MS::kFailure;
	}

	// used to assign materials later
	std::vector<MObject> allGeneratedMeshes;

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
			//Model planeModel;
			//PlaneHelper::planeToModel(currPlane, 1.0f, outCellPositions[i], planeModel);
			//MFnMesh planeMesh;
			//MayaHelper::copyModelToMFnMesh(planeModel, planeMesh);
		}

		if( anyResult ) {
			Model clippedModel;
			if( cm.convert(&clippedModel) ) {
				MFnMesh outCellChunkMesh;
				MayaHelper::copyModelToMFnMesh(clippedModel, outCellChunkMesh);
				allGeneratedMeshes.push_back(outCellChunkMesh.object());
			} else {
				printf("Hadan ERROR: Failed to convert clipped model.\n");
			}
		}
	}

	printf("Hadan Complete (%d/%d chunks)\n", allGeneratedMeshes.size(), outPlaneCounts.size());

	// get and assign the default material to all created meshes
	MSelectionList shadingSelectionList;
	MGlobal::getSelectionListByName("initialShadingGroup", shadingSelectionList);
	MObject shadingGroupObj;
	shadingSelectionList.getDependNode(0, shadingGroupObj);
	MFnSet shadingGroupFn(shadingGroupObj);
	for( auto& mesh : allGeneratedMeshes ) {
		shadingGroupFn.addMember(mesh);
	}

	destroy();

	fflush(stdout);

	return MStatus::kSuccess;
}