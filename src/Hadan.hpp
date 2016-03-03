/**
 * hadan
 *
 * author:
 *    daniel green
 *
 * usage:
 *    [meshName/mn];           string;    Name of object to fracture.
 *    [fractureType/ft];       string;    Type of fracture.  Options: uniform bezier cluster test
 *    [uniformCount/uc];       uint;      Number of uniform points to generate.
 *    [primaryCount/pc];       uint;      Number of primary points to generate.
 *    [secondaryCount/sc];     uint;      Number of secondary points to generate.
 *    [separationDistance/sd]; double;    Distance to move chunks' vertices along their normals.
 *    [fluxPercent/flp];       double;    Percentage relative to the size of the object's bounding volume to flux points by.
 *    [point/pnt];             double x3; Source points.  Can be repeated.
 *
 * Uniform fracturing:
 *    hadan -mn pCube1 -ft uniform -uc 10
 *    [uniformCount/uc]
 *    Simple uniform point generation within the object's bounding box.
 *
 *    [uniformCount/uc] points are randomly generated within the object's bounding box.
 *
 * Bezier fracturing:
 *    hadan -mn pCube1 -ft bezier -uc 10 -flp 15.0 (-pnt 0 0 0 -pnt 1 1 1...)
 *    [uniformCount/uc] [fluxPercent/flp] [point/pnt]
 *    Simulates a propagating crack using a bezier curve and uniform points.
 *
 *    The generation differs based on the number of [point/pnt]s provided.  If four [point/pnt]s are given, they are assumed
 *    to make up a curve.  If two [point/pnt]s are given, they are assumed to the the start and end points respectively, and
 *    the intermediate two are randomly generated within the object's bounding box.  If no [point/pnt]s are provided, then
 *    all four points are randomly generated.  The start and end are generated on the surface of the bounding box, and will
 *    attempt to space themselves [fluxPercent/flp] of the bounding box's total size apart.  If they reach the maximum iteration
 *    count, the last attempt its taken, regardless of the distance.  The two intermediate points are randomly generated as before.
 *    The curve is sampled to generated [CHANGEME] A FIXED NUMBER of points.  These points are then randomized with a distance of
 *    [fluxPercent/flp] of the bounding box's size.  [uniformCount/uc] randomly generated points within the object's bounding box
 *    are also added for variety.
 *
 * Cluster fracturing:
 *    hadan -mn pCube1 -ft cluster -uc 10 -pc 5 -sc 20 -flp 10.0 (-pnt 0 0 0 -pnt 1 1 1...)
 *    [uniformCount/uc] [primaryCount/pc] [secondaryCount/sc] [fluxPercent/flp] [point/pnt]
 *    A set of primary points are used to source local clusters of secondary points, with a sprinkle of uniform on top.
 *
 *    User-provided [point/pnt]s are appended with [primaryCount/pc] randomly generated points within the object's bounding box.
 *    For each of these points, [secondaryCount/sc] points are generated around it.  The distance from the original point is
 *    controlled by [fluxPercent/flp], which is a percentage of the total size of the object's bounding box.
 *    [uniformCount/uc] randomly generated points within the object's bounding box are also added to increase variation.
 */

// todo:
//    - expose minimum distance, as it is hardcoded inside bezier
//    - expose number of samples of bezier curve (currently hardcoded)
//    - fix plane in world space drawing position bug
//    - preserve UVs and somehow add a separate material on the inside
//    - add generated pieces to a group

#ifndef __hadan__
#define __hadan__

#include <maya/MPxCommand.h>
#include <maya/MDagPath.h>
#include <cc/Vec3.hpp>
#include "points/PointGenFactory.hpp"
#include "points/PointGenInfo.hpp"
#include "Model.hpp"
#include "cells/Cell.hpp"
#include "slicing/IMeshSlicer.hpp"
#include <mutex>
#include "slicing/MeshSlicerFactory.hpp"
#include "slicing/MeshSlicerInfo.hpp"

class Hadan : public MPxCommand {
public:
	Hadan();
	virtual ~Hadan();

	virtual MStatus doIt( const MArgList& args ) override;
	virtual MStatus undoIt() override;
	virtual MStatus redoIt() override;
	virtual bool isUndoable() const override;
	virtual bool hasSyntax() const override;

private:
	bool parseArgs( const MArgList& args );
	bool validateInputMesh() const;
	void copyMeshFromMaya();
	bool generateSamplePoints();
	bool generateCuttingCells();
	void doSingleCut( const Cell& cell, int id, std::shared_ptr<IMeshSlicer> slicer );
	void performCutting();
	void centerAllPivots();
	void applyMaterials();
	void separateCells();
	void restoreInitialSelection();

private:
	MDagPath _inputMesh;
	PointGenFactory::Type _pointsGenType;
	double _separationDistance;
	PointGenInfo _pointGenInfo;
	std::vector<cc::Vec3f> _samplePoints;
	std::vector<Cell> _cuttingCells;
	std::vector<MObject> _generatedMeshes;
	BoundingBox _boundingBox;
	bool _useMultithreading;
	MeshSlicerInfo _meshSlicerInfo;
	MeshSlicerFactory::Type _slicerType;
};

#endif /* __hadan__ */