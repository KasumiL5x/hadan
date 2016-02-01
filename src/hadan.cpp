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
#include "MTLog.hpp"
#include <thread>
//#include "ProgressHelper.hpp"

static std::mutex GeneratedMeshesMutex;

Hadan::Hadan()
	: MPxCommand(), _inputMesh(), _pointsGenType(PointGenFactory::Type::Invalid), _separationDistance(0.0), _pointGenInfo() {
}

Hadan::~Hadan() {
}

MStatus Hadan::doIt( const MArgList& args ) {
	// initialize progress counter
	//if( !ProgressHelper::init() ) {
	//	Log::error("Error: Failed to reserve progress window; it is already in use.\n");
	//	return MS::kFailure;
	//}

	// get start time
	const auto startTime = std::chrono::system_clock::now();

	// print start time
	const std::time_t epochTime = std::chrono::system_clock::to_time_t(startTime);
	char startTimeStr[100];
	std::strftime(startTimeStr, sizeof(startTimeStr), "%X", std::localtime(&epochTime));
	MTLog::instance()->log("Hadan starting at " + std::string(startTimeStr) + "\n");

	// parse incoming arguments
	if( !parseArgs(args) ) {
		MTLog::instance()->log("Error: Failed to parse arguments.\n");
		//ProgressHelper::end();
		return MS::kFailure;
	}

	// validate input mesh
	if( !validateInputMesh() ) {
		MTLog::instance()->log("Error: Failed to validate mesh.\n");
		//ProgressHelper::end();
		return MS::kFailure;
	}

	// copy maya mesh into local model
	copyMeshFromMaya();

	// generate sample points
	if( !generateSamplePoints() ) {
		MTLog::instance()->log("Error: Not enough sample points were generated.\n");
		//ProgressHelper::end();
		return MS::kFailure;
	}

	// generating cutting cells
	if( !generateCuttingCells() ) {
		MTLog::instance()->log("Error: Generated cutting cells were inadequate.\n");
		//ProgressHelper::end();
		return MS::kFailure;
	}

	// cut out all cells, creating a new piece of geometry for each
	performCutting();

	// center pivots of all chunks
	centerAllPivots();

	// soften edges of all objects
	softenAllEdges();

	// apply default material to all generated cells
	applyMaterials();

	// shrink vertices of chunks along normals
	separateCells();

	// end with only the source mesh selected (for easy deleting, etc.)
	restoreInitialSelection();

	// print completion stats
	const auto endTime = std::chrono::system_clock::now();
	const std::chrono::duration<double> timeDiff = endTime - startTime;
	const std::string timeTakenStr = "Hadan finished in " + std::to_string(timeDiff.count()) + "s. ";
	const std::string chunkStr = std::to_string(_generatedMeshes.size()) + "/" + std::to_string(_cuttingCells.size()) + " chunks generated.\n";
	MTLog::instance()->log(timeTakenStr + chunkStr);

	// end progress window
	//ProgressHelper::end();

	return MStatus::kSuccess;
}

MStatus Hadan::undoIt() {
	MTLog::instance()->log("Warning: Hadan::undoIt() not yet implemented.\n");
	return MStatus::kFailure;
}

MStatus Hadan::redoIt() {
	MTLog::instance()->log("Warning: Hadan::redoIt() not yet implemented.\n");
	return MStatus::kFailure;
}

bool Hadan::isUndoable() const {
	return true;
}

bool Hadan::hasSyntax() const {
	return true;
}

bool Hadan::parseArgs( const MArgList& args ) {
	//ProgressHelper::begin(9, "Parsing input...");

	// read input args from database with syntax
	const MArgDatabase db(HadanArgs::Syntax(), args);

	// clear existing arg values
	_inputMesh = MDagPath();
	_pointsGenType = PointGenFactory::Type::Invalid;
	_separationDistance = 0.0;
	_pointGenInfo = PointGenInfo();

	// parse and validate existance of mesh name
	if( !db.isFlagSet(HadanArgs::MeshName) ) {
		MTLog::instance()->log("Error: Required argument -meshname (-mn) is missing.\n");
		return false;
	}
	MString meshNameStr;
	db.getFlagArgument(HadanArgs::MeshName, 0, meshNameStr);
	if( !MayaHelper::getObjectFromString(meshNameStr.asChar(), _inputMesh) ) {
		MTLog::instance()->log("Error: Given object not found.\n");
		return false;
	}
	//ProgressHelper::advance();

	// parse fracture type
	if( !db.isFlagSet(HadanArgs::FractureType) ) {
		MTLog::instance()->log("Error: Required argument -fracturetype (-ft) is missing.\n");
		return false;
	}
	MString fractureTypeStr;
	db.getFlagArgument(HadanArgs::FractureType, 0, fractureTypeStr);
	if( strcmp(fractureTypeStr.asChar(), "uniform") == 0 ) {
		_pointsGenType = PointGenFactory::Type::Uniform;
	} else if( strcmp(fractureTypeStr.asChar(), "bezier") == 0 ) {
		_pointsGenType = PointGenFactory::Type::Bezier;
	} else if( strcmp(fractureTypeStr.asChar(), "cluster") == 0 ) {
		_pointsGenType = PointGenFactory::Type::Cluster;
	} else if( strcmp(fractureTypeStr.asChar(), "test") == 0 ) {
		_pointsGenType = PointGenFactory::Type::Test;
	} else {
		MTLog::instance()->log("Error: Unknown fracture type.\n");
		return false;
	}
	//ProgressHelper::advance();

	// parse separation distance
	db.getFlagArgument(HadanArgs::SeparateDistance, 0, _separationDistance);
	//ProgressHelper::advance();

	// parse uniform count
	db.getFlagArgument(HadanArgs::UniformCount, 0, _pointGenInfo.uniformCount);
	//ProgressHelper::advance();

	// parse primary count
	db.getFlagArgument(HadanArgs::PrimaryCount, 0, _pointGenInfo.primaryCount);
	//ProgressHelper::advance();

	// parse secondary count
	db.getFlagArgument(HadanArgs::SecondaryCount, 0, _pointGenInfo.secondaryCount);
	//ProgressHelper::advance();

	// parse sample count
	db.getFlagArgument(HadanArgs::Samples, 0, _pointGenInfo.samples);
	//ProgressHelper::advance();

	// parse flux
	db.getFlagArgument(HadanArgs::FluxPercentage, 0, _pointGenInfo.flux);
	//ProgressHelper::advance();

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
		_pointGenInfo.userPoints.push_back(cc::Vec3f(static_cast<float>(vector.x), static_cast<float>(vector.y), static_cast<float>(vector.z)));
	}
	//ProgressHelper::advance();

	return true;
}

bool Hadan::validateInputMesh() const {
	//ProgressHelper::begin(3, "Validating mesh...");

	MFnMesh mesh(_inputMesh);

	// check it is a mesh
	if( !MayaHelper::hasMesh(_inputMesh) ) {
		MTLog::instance()->log("Error: Input object is not a mesh.\n");
		return false;
	}
	//ProgressHelper::advance();

	// check that the mesh does not have any holes
	if( MayaHelper::doesMeshHaveHoles(mesh) ) {
		MTLog::instance()->log("Error: Mesh cannot have holes.\n");
		return false;
	}
	//ProgressHelper::advance();

	// ensure the that mesh is fully closed (all edges must have two faces)
	if( !MayaHelper::isMeshFullyClosed(_inputMesh) ) {
		MTLog::instance()->log("Error: Mesh is not closed.  All edges must have two faces.\n");
		return false;
	}
	//ProgressHelper::advance();

	return true;
}

void Hadan::copyMeshFromMaya() {
	//ProgressHelper::begin(0, "Copying mesh...");

	MayaHelper::copyMFnMeshToModel(MFnMesh(_inputMesh), _modelFromMaya);
	_modelFromMaya.buildExtendedData();
}

bool Hadan::generateSamplePoints() {
	//ProgressHelper::begin(0, "Generating points...");

	std::unique_ptr<IPointGen> gen = PointGenFactory::create(_pointsGenType);
	gen->generateSamplePoints(_modelFromMaya, _pointGenInfo, _samplePoints);
	return !_samplePoints.empty();
}

bool Hadan::generateCuttingCells() {
	//ProgressHelper::begin(0, "Generating cells...");

	std::unique_ptr<ICellGen> gen = CellGenFactory::create(CellGenFactory::Type::Voronoi);
	gen->generate(_modelFromMaya.computeBoundingBox(), _samplePoints, _cuttingCells);
	return !_cuttingCells.empty();
}

void Hadan::doSingleCut( const Cell& cell, int id, std::shared_ptr<IMeshSlicer> slicer ) {
	Model outModel;
	if( !slicer->slice(_modelFromMaya, cell, outModel) ) {
		//Log::warning("Warning: Failed to slice using cell " + std::to_string(id) + ".  This is sometimes expected.\n");
		return;
	}

	std::lock_guard<std::mutex> lk(GeneratedMeshesMutex);
	_generatedModels.push_back(outModel);
}

void Hadan::performCutting() {
	//ProgressHelper::begin(static_cast<int>(_cuttingCells.size()), "Slicing geometry...");

	//std::unique_ptr<IMeshSlicer> slicer = MeshSlicerFactory::create(MeshSlicerFactory::Type::ClosedConvex);
	std::shared_ptr<IMeshSlicer> slicer = std::make_shared<ClosedConvexSlicer>();
	std::vector<std::thread> cuttingThreads;
	for( unsigned int i = 0; i < static_cast<unsigned int>(_cuttingCells.size()); ++i ) {
		cuttingThreads.push_back(std::thread(&Hadan::doSingleCut, this, _cuttingCells[i], i, slicer));
	}
	for( auto& t : cuttingThreads ) {
		t.join();
	}
	for( auto& mdl : _generatedModels ) {
		MFnMesh outMesh;
		MayaHelper::copyModelToMFnMesh(mdl, outMesh);
		_generatedMeshes.push_back(outMesh.object());
	}
}

void Hadan::centerAllPivots() {
	//ProgressHelper::begin(static_cast<int>(_generatedMeshes.size()), "Centering pivots...");

	for( const auto& mesh : _generatedMeshes ) {
		const std::string meshName = std::string(MFnMesh(mesh).fullPathName().asChar());
		MayaHelper::centerPivot(meshName);

		//ProgressHelper::advance();
	}
}

void Hadan::softenAllEdges() {
	//ProgressHelper::begin(static_cast<int>(_generatedMeshes.size()), "Softening edges...");

	for( const auto& mesh : _generatedMeshes ) {
		const std::string meshName = std::string(MFnMesh(mesh).fullPathName().asChar());
		MayaHelper::softenMeshEdge(meshName);

		//ProgressHelper::advance();
	}
}

void Hadan::applyMaterials() {
	//ProgressHelper::begin(static_cast<int>(_generatedMeshes.size()), "Applying materials...");

	MSelectionList shadingSelectionList;
	MGlobal::getSelectionListByName("initialShadingGroup", shadingSelectionList);
	MObject shadingGroupObj;
	shadingSelectionList.getDependNode(0, shadingGroupObj);
	MFnSet shadingGroupFn(shadingGroupObj);
	for( const auto& mesh : _generatedMeshes ) {
		shadingGroupFn.addMember(mesh);

		//ProgressHelper::advance();
	}
}

void Hadan::separateCells() {
	//ProgressHelper::begin(static_cast<int>(_generatedMeshes.size()), "Separating cells...");

	if( !cc::math::equal<double>(_separationDistance, 0.0) ) {
		for( const auto& mesh : _generatedMeshes ) {
			MayaHelper::moveVerticesAlongNormal(mesh, _separationDistance, true);

			//ProgressHelper::advance();
		}
	}
}

void Hadan::restoreInitialSelection() {
	MSelectionList sourceObjectSelectionList;
	sourceObjectSelectionList.add(_inputMesh);
	MGlobal::setActiveSelectionList(sourceObjectSelectionList);
}