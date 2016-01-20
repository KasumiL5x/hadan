/**
 * hadan
 *
 * author:
 *    daniel green
 *
 * usage:
 *    hadan <mesh_name> <slice_count> <separate_dist>
 *    <mesh_name>     is a string representing the path of the mesh to fracture
 *    <slice_count>   is the number of slices the source geometry should be split into (must be >=1)
 *    <separate_dist> is how far to push chunks' vertices along their normal to separate geometry (can be zero)
 */

// todo:
//    - fix plane in world space drawing position bug
//    - preserve UVs and somehow add a separate material on the inside
//    - add generated pieces to a group

#ifndef __hadan__
#define __hadan__

#include <maya/MPxCommand.h>
#include <maya/MDagPath.h>
#include <cc/Vec3.hpp>
#include "points/PointGenFactory.hpp"

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

private:
	MDagPath _inputMesh;
	unsigned int _sliceCount;
	PointGenFactory::Type _pointsGenType;
	double _separationDistance;
	std::vector<cc::Vec3f> _userPoints;
	double _flux;
};

#endif /* __hadan__ */