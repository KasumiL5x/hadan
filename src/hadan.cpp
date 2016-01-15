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
//    - clean up code (and properly document classes
//    - make cutting also an interface
//    - format printing in a coherent fashion
//    - fix plane in world space drawing position bug
//    - add ability to push vertices along negative normals by a fraction to shrink mesh (once normals are generated properly), mainly for stacking
//    - preserve UVs and somehow add a separate material on the inside

#include <chrono>
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
#include "Log.hpp"

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
	// get start time
	const auto startTime = std::chrono::system_clock::now();

	// print start time
	const std::time_t epochTime = std::chrono::system_clock::to_time_t(startTime);
	//const std::string startTimeStr = std::ctime(&epochTime);
	char startTimeStr[100];
	std::strftime(startTimeStr, sizeof(startTimeStr), "%X", std::localtime(&epochTime));
	Log::info("Hadan starting at " + std::string(startTimeStr) + "\n");

	// randomize seed
	srand(static_cast<unsigned int>(startTime.time_since_epoch().count()));

	// check arguments length
	if( args.length() < 2 ) {
		Log::error("Error: Incorrect number of arguments.\n");
		return MS::kFailure;
	}

	// get and validate number of slices
	const int numSlices = args.asInt(1);
	if( numSlices < 1 ) {
		Log::error("Error: Must have at least one slice!\n");
		return MS::kFailure;
	}

	// get the input object's dag path and check that it exists
	MDagPath inputObjectPath;
	if( !MayaHelper::getObjectFromString(args.asString(0).asChar(), inputObjectPath) ) {
		Log::error("Error: Given object not found.\n");
		return MS::kFailure;
	}

	// check that the input object has a mesh fn
	if( !MayaHelper::hasMesh(inputObjectPath) ) {
		Log::error("Error: Given object is not a mesh.\n");
		return MS::kFailure;
	}

	// convert the input object to a mesh and check if it has holes
	MFnMesh mayaMesh(inputObjectPath);
	if( MayaHelper::doesMeshHaveHoles(mayaMesh) ) {
		Log::error("Error: Mesh cannot have holes.\n");
		return MS::kFailure;
	}

	// ensure that the mesh is fully closed (all edges must have two faces)
	if( !MayaHelper::isMeshFullyClosed(inputObjectPath) ) {
		Log::error("Error: Mesh is not closed.  An edge somewhere has only a single face.\n");
		return MS::kFailure;
	}

	// copy maya mesh into custom model and build extended data
	Model fromMaya;
	MayaHelper::copyMFnMeshToModel(mayaMesh, fromMaya);
	fromMaya.buildExtendedData();

	// create a sample point generator and generate sample points
	pointGenerator = createSamplePointGenerator();
	std::vector<cc::Vec3f> samplePoints;
	pointGenerator->generateSamplePoints(fromMaya, static_cast<unsigned int>(numSlices), samplePoints);

	if( samplePoints.empty() ) {
		Log::error("Error: Not enough sample points were generated.\n");
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
		Log::error("Error: Generated cutting planes were inadequate.\n");
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
				Log::info("Failed to convert clipped mesh, most likely due to clipping leaving insignificant geometry.  This is expected.\n");
			}
		}
	}

	// modify normals of all meshes (harden then set angle as sometimes only set angle doesn't work)
	for( const auto& mesh : allGeneratedMeshes ) {
		const std::string meshName = std::string(MFnMesh(mesh).fullPathName().asChar());

		// harden edges
		const std::string harden = "polySoftEdge -a 0 -ch 1 " + meshName + ";";
		MGlobal::executeCommand(harden.c_str());
		// set normal angle to 30
		const std::string setNormalAngle = "polySoftEdge -angle 30 -ch 1 " + meshName + ";";
		MGlobal::executeCommand(setNormalAngle.c_str());
	}

	// get and assign the default material to all created meshes
	MSelectionList shadingSelectionList;
	MGlobal::getSelectionListByName("initialShadingGroup", shadingSelectionList);
	MObject shadingGroupObj;
	shadingSelectionList.getDependNode(0, shadingGroupObj);
	MFnSet shadingGroupFn(shadingGroupObj);
	for( auto& mesh : allGeneratedMeshes ) {
		shadingGroupFn.addMember(mesh);
	}

	// end with only the source mesh selected (for easy deleting, etc.)
	MSelectionList sourceObjectSelectionList;
	sourceObjectSelectionList.add(inputObjectPath);
	MGlobal::setActiveSelectionList(sourceObjectSelectionList);


	// print completion stats
	const auto endTime = std::chrono::system_clock::now();
	const std::chrono::duration<double> timeDiff = endTime - startTime;
	const std::string timeTakenStr = "Hadan finished in " + std::to_string(timeDiff.count()) + "s. ";
	const std::string chunkStr = std::to_string(allGeneratedMeshes.size()) + "/" + std::to_string(outPlaneCounts.size()) + " chunks generated.\n";
	Log::info(timeTakenStr + chunkStr);

	destroy();

	fflush(stdout);

	return MStatus::kSuccess;
}