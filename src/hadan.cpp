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
#include "slicing/CSGSlicer/CSGSlicer.hpp"

static std::mutex GeneratedMeshesMutex;

Hadan::Hadan()
	: MPxCommand(), _inputMesh(), _pointsGenType(PointGenFactory::Type::Invalid), _separationDistance(0.0), _pointGenInfo(), _useMultithreading(false) {
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
	MTLog::instance()->log("Hadan starting at " + std::string(startTimeStr) + "\n");

	// parse incoming arguments
	if( !parseArgs(args) ) {
		MTLog::instance()->log("Error: Failed to parse arguments.\n");
		return MS::kFailure;
	}

	// clear all selections as some MEL commands dislike things being selected
	MGlobal::clearSelectionList();

	// validate input mesh
	if( !validateInputMesh() ) {
		MTLog::instance()->log("Error: Failed to validate mesh.\n");
		return MS::kFailure;
	}

	// get the bounding box from Maya
	_boundingBox = MayaHelper::getBoundingBox(MFnMesh(_inputMesh));

	// generate sample points
	if( !generateSamplePoints() ) {
		MTLog::instance()->log("Error: Not enough sample points were generated.\n");
		return MS::kFailure;
	}

	// generating cutting cells
	if( !generateCuttingCells() ) {
		MTLog::instance()->log("Error: Generated cutting cells were inadequate.\n");
		return MS::kFailure;
	}

	// cut out all cells, creating a new piece of geometry for each
	performCutting();

	// clear selection
	MGlobal::clearSelectionList();

	// center all selected objects' pivots
	centerAllPivots();

	// apply default material to all generated cells
	applyMaterials();

	// create parent group for the shards
	MFnTransform parentXform;
	parentXform.create();
	parentXform.setName(MFnDagNode(_inputMesh).name() + "_chunks");

	// parent all chunks to the parent xform (this will make a new one if it already exists like maya does)
	for( auto& curr : _generatedMeshes ) {
		parentXform.addChild(MFnDagNode(curr).parent(0));
	}

	// shrink vertices of chunks along normals
	//separateCells();

	// end with only the source mesh selected (for easy deleting, etc.)
	restoreInitialSelection();

	// print completion stats
	const auto endTime = std::chrono::system_clock::now();
	const std::chrono::duration<double> timeDiff = endTime - startTime;
	const std::string timeTakenStr = "Hadan finished in " + std::to_string(timeDiff.count()) + "s. ";
	const std::string chunkStr = std::to_string(_generatedMeshes.size()) + "/" + std::to_string(_cuttingCells.size()) + " chunks generated.\n";
	MTLog::instance()->log(timeTakenStr + chunkStr);

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
	// read input args from database with syntax
	const MArgDatabase db(HadanArgs::Syntax(), args);

	// clear existing arg values
	_inputMesh = MDagPath();
	_pointsGenType = PointGenFactory::Type::Invalid;
	_separationDistance = 0.0;
	_pointGenInfo = PointGenInfo();

	// parse and validate existance of mesh name
	if( !db.isFlagSet(HadanArgs::HadanMeshName) ) {
		MTLog::instance()->log("Error: Required argument -meshname (-mn) is missing.\n");
		return false;
	}
	MString meshNameStr;
	db.getFlagArgument(HadanArgs::HadanMeshName, 0, meshNameStr);
	if( !MayaHelper::getObjectFromString(meshNameStr.asChar(), _inputMesh) ) {
		MTLog::instance()->log("Error: Given object not found.\n");
		return false;
	}

	// parse fracture type
	if( !db.isFlagSet(HadanArgs::HadanFractureType) ) {
		MTLog::instance()->log("Error: Required argument -fracturetype (-ft) is missing.\n");
		return false;
	}
	MString fractureTypeStr;
	db.getFlagArgument(HadanArgs::HadanFractureType, 0, fractureTypeStr);
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

	// parse separation distance
	db.getFlagArgument(HadanArgs::HadanSeparateDistance, 0, _separationDistance);

	// parse uniform count
	db.getFlagArgument(HadanArgs::HadanUniformCount, 0, _pointGenInfo.uniformCount);

	// parse primary count
	db.getFlagArgument(HadanArgs::HadanPrimaryCount, 0, _pointGenInfo.primaryCount);

	// parse secondary count
	db.getFlagArgument(HadanArgs::HadanSecondaryCount, 0, _pointGenInfo.secondaryCount);

	// parse sample count
	db.getFlagArgument(HadanArgs::HadanSamples, 0, _pointGenInfo.samples);

	// parse flux
	db.getFlagArgument(HadanArgs::HadanFluxPercentage, 0, _pointGenInfo.flux);

	// parse random seed
	db.getFlagArgument(HadanArgs::HadanRandomSeed, 0, _pointGenInfo.seed);

	// parse smoothing angle
	if( db.isFlagSet(HadanArgs::HadanSmoothingAngle) ) {
		db.getFlagArgument(HadanArgs::HadanSmoothingAngle, 0, _meshSlicerInfo.smoothingAngle);
	}

	// parse bezier min distance
	if( db.isFlagSet(HadanArgs::HadanBezierMinDist) ) {
		db.getFlagArgument(HadanArgs::HadanBezierMinDist, 0, _pointGenInfo.minBezierDistance);
		_pointGenInfo.minBezierDistance = cc::math::clamp<double>(_pointGenInfo.minBezierDistance, 0.0, 100.0);
	}

	// parse multi-threading
	if( db.isFlagSet(HadanArgs::HadanMultiThreading) ) {
		db.getFlagArgument(HadanArgs::HadanMultiThreading, 0, _useMultithreading);
	}

	// parse user's optional points list
	const unsigned int pntUses = db.numberOfFlagUses(HadanArgs::HadanPoint);
	for( unsigned int i = 0; i < pntUses; ++i ) {
		MArgList pntArgsList;
		db.getFlagArgumentList(HadanArgs::HadanPoint, i, pntArgsList);
		if( pntArgsList.length() != 3 ) {
			MTLog::instance()->log("Ignoring -pnt (-point) " + std::to_string(i) + " because it was formatted incorrectly.\n");
			continue;
		}
		unsigned int dummyIndex = 0;
		const MVector vector = pntArgsList.asVector(dummyIndex, 3);
		_pointGenInfo.userPoints.push_back(cc::Vec3f(static_cast<float>(vector.x), static_cast<float>(vector.y), static_cast<float>(vector.z)));
	}

	return true;
}

bool Hadan::validateInputMesh() const {
	MFnMesh mesh(_inputMesh);

	// check it is a mesh
	if( !MayaHelper::hasMesh(_inputMesh) ) {
		MTLog::instance()->log("Error: Input object is not a mesh.\n");
		return false;
	}

	// check that the mesh does not have any holes
	if( MayaHelper::doesMeshHaveHoles(mesh) ) {
		MTLog::instance()->log("Error: Mesh cannot have holes.\n");
		return false;
	}

	// ensure the that mesh is fully closed (all edges must have two faces)
	if( !MayaHelper::isMeshFullyClosed(_inputMesh) ) {
		MTLog::instance()->log("Error: Mesh is not closed.  All edges must have two faces.\n");
		return false;
	}

	return true;
}

bool Hadan::generateSamplePoints() {
	std::unique_ptr<IPointGen> gen = PointGenFactory::create(_pointsGenType);
	gen->generateSamplePoints(_boundingBox, _pointGenInfo, _samplePoints);
	return !_samplePoints.empty();
}

bool Hadan::generateCuttingCells() {
	std::unique_ptr<ICellGen> gen = CellGenFactory::create(CellGenFactory::Type::Voronoi);
	gen->generate(_boundingBox, _samplePoints, _cuttingCells);
	return !_cuttingCells.empty();
}

void Hadan::doSingleCut( const Cell& cell, int id, std::shared_ptr<IMeshSlicer> slicer ) {
	MFnMesh outMesh;
	if( !slicer->slice(cell, _meshSlicerInfo, outMesh) ) {
		MTLog::instance()->log("Warning: Failed to slice using cell " + std::to_string(id) + ".  This is sometimes expected.\n");
		return;
	}

	std::lock_guard<std::mutex> lk(GeneratedMeshesMutex);
	_generatedMeshes.push_back(outMesh.object());
}

void Hadan::performCutting() {
	std::shared_ptr<IMeshSlicer> slicer = std::make_shared<ClosedConvexSlicer>();
	//std::shared_ptr<IMeshSlicer> slicer = std::make_shared<CSGSlicer>();
	if( !slicer->setSource(MFnMesh(_inputMesh)) ) {
		MTLog::instance()->log("Warning: Failed to set slicer mesh source.  Cutting will not take place.\n");
		return;
	}

	if( _useMultithreading ) {
		// multi threaded
		std::vector<std::thread> cuttingThreads;
		for( unsigned int i = 0; i < static_cast<unsigned int>(_cuttingCells.size()); ++i ) {
			cuttingThreads.push_back(std::thread(&Hadan::doSingleCut, this, _cuttingCells[i], i, slicer));
		}
		for( auto& t : cuttingThreads ) {
			t.join();
		}
	} else {
		// single threaded
		for( unsigned int i = 0; i < static_cast<unsigned int>(_cuttingCells.size()); ++i ) {
			doSingleCut(_cuttingCells[i], i, slicer);
		}
	}
}

void Hadan::centerAllPivots() {
	for( const auto& mesh : _generatedMeshes ) {
		const std::string meshName = std::string(MFnMesh(mesh).fullPathName().asChar());
		MayaHelper::centerPivot(meshName);
	}
}

void Hadan::applyMaterials() {
	MSelectionList shadingSelectionList;
	MGlobal::getSelectionListByName("initialShadingGroup", shadingSelectionList);
	MObject shadingGroupObj;
	shadingSelectionList.getDependNode(0, shadingGroupObj);
	MFnSet shadingGroupFn(shadingGroupObj);
	for( const auto& mesh : _generatedMeshes ) {
		shadingGroupFn.addMember(mesh);
	}
}

void Hadan::separateCells() {
	if( !cc::math::equal<double>(_separationDistance, 0.0) ) {
		for( const auto& mesh : _generatedMeshes ) {
			MayaHelper::moveVerticesAlongNormal(mesh, _separationDistance, true);
		}
	}
}

void Hadan::restoreInitialSelection() {
	MSelectionList sourceObjectSelectionList;
	sourceObjectSelectionList.add(_inputMesh);
	MGlobal::setActiveSelectionList(sourceObjectSelectionList);
}