#include "Hadan.hpp"
#include <chrono>
#include <ctime>
#include <memory>
#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MDagPath.h>
#include "Syntax.hpp"
#include "MayaHelper.hpp"
#include "points/PointGenFactory.hpp"
#include "cells/CellGenFactory.hpp"
#include "slicing/MeshSlicerFactory.hpp"
#include <maya/MFnSet.h>
#include "Log.hpp"

Hadan::Hadan()
	: MPxCommand() {
}

Hadan::~Hadan() {
}

MStatus Hadan::doIt( const MArgList& args ) {
	// get start time
	const auto startTime = std::chrono::system_clock::now();

	// print start time
	const std::time_t epochTime = std::chrono::system_clock::to_time_t(startTime);
	char startTimeStr[100];
	std::strftime(startTimeStr, sizeof(startTimeStr), "%X", std::localtime(&epochTime));
	Log::info("Hadan starting at " + std::string(startTimeStr) + "\n");

	// parse incoming arguments
	if( !parseArgs(args) ) {
		Log::error("Error: Failed to parse arguments.\n");
		return MS::kFailure;
	}

	// randomize seed
	srand(static_cast<unsigned int>(startTime.time_since_epoch().count()));

	// convert the input object to a mesh and check if it has holes
	MFnMesh mayaMesh(_inputMesh);
	if( MayaHelper::doesMeshHaveHoles(mayaMesh) ) {
		Log::error("Error: Mesh cannot have holes.\n");
		return MS::kFailure;
	}

	// ensure that the mesh is fully closed (all edges must have two faces)
	if( !MayaHelper::isMeshFullyClosed(_inputMesh) ) {
		Log::error("Error: Mesh is not closed.  An edge somewhere has only a single face.\n");
		return MS::kFailure;
	}

	// copy maya mesh into custom model and build extended data
	Model fromMaya;
	MayaHelper::copyMFnMeshToModel(mayaMesh, fromMaya);
	fromMaya.buildExtendedData();

	// create a sample point generator and generate sample points
	std::unique_ptr<IPointGen> pointGenerator = PointGenFactory::create(_pointsGenType);
	std::vector<cc::Vec3f> samplePoints;
	pointGenerator->setUserPoints(_userPoints);
	pointGenerator->generateSamplePoints(fromMaya, _sliceCount, samplePoints);

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

	// shrink vertices of chunks along normals
	if( _separationDistance != 0.0 ) {
		for( auto& mesh : allGeneratedMeshes ) {
			MayaHelper::moveVerticesAlongNormal(mesh, _separationDistance, true);
		}
	}

	// end with only the source mesh selected (for easy deleting, etc.)
	MSelectionList sourceObjectSelectionList;
	sourceObjectSelectionList.add(_inputMesh);
	MGlobal::setActiveSelectionList(sourceObjectSelectionList);

	// print completion stats
	const auto endTime = std::chrono::system_clock::now();
	const std::chrono::duration<double> timeDiff = endTime - startTime;
	const std::string timeTakenStr = "Hadan finished in " + std::to_string(timeDiff.count()) + "s. ";
	const std::string chunkStr = std::to_string(allGeneratedMeshes.size()) + "/" + std::to_string(outCells.size()) + " chunks generated.\n";
	Log::info(timeTakenStr + chunkStr);

	return MStatus::kSuccess;
}

MStatus Hadan::undoIt() {
	Log::warning("Hadan::undoIt() not yet implemented.\n");
	return MStatus::kFailure;
}

MStatus Hadan::redoIt() {
	Log::warning("Hadan::redoIt() not yet implemented.\n");
	return MStatus::kFailure;
}

bool Hadan::isUndoable() const {
	return true;
}

bool Hadan::hasSyntax() const {
	return true;
}

bool Hadan::parseArgs( const MArgList& args ) {
	// read input args from database with syntax
	const MArgDatabase db(HadanArgs::Syntax(), args);

	// clear existing arg values
	_inputMesh = MDagPath();
	_sliceCount = 0;
	_pointsGenType = PointGenFactory::Type::Invalid;
	_separationDistance = 0.0;
	_userPoints.clear();

	// parse and validate mesh name
	if( !db.isFlagSet(HadanArgs::MeshName) ) {
		Log::error("Error: Required argument -meshname (-mn) is missing.\n");
		return false;
	}
	MString meshNameStr;
	db.getFlagArgument(HadanArgs::MeshName, 0, meshNameStr);
	if( !MayaHelper::getObjectFromString(meshNameStr.asChar(), _inputMesh) ) {
		Log::error("Error: Given object not found.\n");
		return false;
	}
	if( !MayaHelper::hasMesh(_inputMesh) ) {
		Log::error("Error: Given object is not a mesh.\n");
		return false;
	}

	// parse and validate slice count
	if( !db.isFlagSet(HadanArgs::SliceCount) ) {
		Log::error("Error: Required argument -slicecount (-sc) is missing.\n");
		return false;
	}
	db.getFlagArgument(HadanArgs::SliceCount, 0, _sliceCount);
	if( 0 == _sliceCount ) {
		Log::error("Error: Must have at least one slice.\n");
		return false;
	}

	// parse fracture type
	if( !db.isFlagSet(HadanArgs::FractureType) ) {
		Log::error("Error: Required argument -fracturetype (-ft) is missing.\n");
		return false;
	}
	MString fractureTypeStr;
	db.getFlagArgument(HadanArgs::FractureType, 0, fractureTypeStr);
	if( strcmp(fractureTypeStr.asChar(), "uniform") == 0 ) {
		_pointsGenType = PointGenFactory::Type::Uniform;
	} else if( strcmp(fractureTypeStr.asChar(), "bezier") == 0 ) {
		_pointsGenType = PointGenFactory::Type::Bezier;
	} else if( strcmp(fractureTypeStr.asChar(), "test") == 0 ) {
		_pointsGenType = PointGenFactory::Type::Test;
	} else {
		Log::error("Error: Unknown fracture type.\n");
		return false;
	}

	// parse separation distance
	if( !db.isFlagSet(HadanArgs::SeparateDistance) ) {
		Log::info("Argument -separationdistance (-sd) was missing; using default value of 0.\n");
		_separationDistance = 0.0;
	} else {
		db.getFlagArgument(HadanArgs::SeparateDistance, 0, _separationDistance);
	}

	// parse user's optional points list
	const unsigned int pntUses = db.numberOfFlagUses(HadanArgs::Point);
	for( unsigned int i = 0; i < pntUses; ++i ) {
		MArgList pntArgsList;
		db.getFlagArgumentList(HadanArgs::Point, i, pntArgsList);
		if( pntArgsList.length() != 3 ) {
			printf("Ignoring -pnt (-point) %d because it was formatted incorrectly.\n", i);
			continue;
		}
		unsigned int dummyIndex = 0;
		const MVector vector = pntArgsList.asVector(dummyIndex, 3);
		_userPoints.push_back(cc::Vec3f(static_cast<float>(vector.x), static_cast<float>(vector.y), static_cast<float>(vector.z)));
		//printf("Parsed vector %d: %f, %f, %f\n", i, vector.x, vector.y, vector.z);
	}

	return true;
}
