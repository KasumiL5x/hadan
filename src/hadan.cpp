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
#include <slicing/ClipMesh.hpp>
#include "MayaHelper.hpp"
#include <cells/CellGenFactory.hpp>
#include <points/PointGenFactory.hpp>
#include <slicing/MeshSlicerFactory.hpp>
#include "Log.hpp"

DeclareSimpleCommand(hadan, "KasumiL5x", "0.0.1-dev");

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
	std::unique_ptr<IPointGen> pointGenerator = PointGenFactory::create(PointGenFactory::Type::Test);
	std::vector<cc::Vec3f> samplePoints;
	pointGenerator->generateSamplePoints(fromMaya, static_cast<unsigned int>(numSlices), samplePoints);

	if( samplePoints.empty() ) {
		Log::error("Error: Not enough sample points were generated.\n");
		return MS::kFailure;
	}

	// create a plane generator and generate cutting planes
	std::unique_ptr<ICellGen> cellGenerator = CellGenFactory::create(CellGenFactory::Type::Voronoi);
	std::vector<Cell> outCells;
	cellGenerator->generate(fromMaya.computeBoundingBox(), samplePoints, outCells);

	if( outCells.empty() ) {
		Log::error("Error: Generated cells were inadequate.\n");
		return MS::kFailure;
	}

	// used to assign materials later
	std::vector<MObject> allGeneratedMeshes;

	// create a mesh slicer
	std::unique_ptr<IMeshSlicer> meshSlicer = MeshSlicerFactory::create(MeshSlicerFactory::Type::ClosedConvex);

	// cut out all cells creating a new piece of geometry for each
	for( unsigned int i = 0; i < static_cast<unsigned int>(outCells.size()); ++i ) {
		const Cell& currCell = outCells[i];
		Model outModel;
		if( !meshSlicer->slice(fromMaya, currCell, outModel) ) {
			Log::warning("Warning: Failed to slice using cell " + std::to_string(i) + ".  This is sometimes expected.\n");
			continue;
		}
		MFnMesh outCellMesh;
		MayaHelper::copyModelToMFnMesh(outModel, outCellMesh);
		allGeneratedMeshes.push_back(outCellMesh.object());
	}

	// run mel commands on the generated chunks
	for( const auto& mesh : allGeneratedMeshes ) {
		const std::string meshName = std::string(MFnMesh(mesh).fullPathName().asChar());
		MayaHelper::softenMeshEdge(meshName);
		MayaHelper::centerPivot(meshName);
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
	const std::string chunkStr = std::to_string(allGeneratedMeshes.size()) + "/" + std::to_string(outCells.size()) + " chunks generated.\n";
	Log::info(timeTakenStr + chunkStr);

	return MStatus::kSuccess;
}