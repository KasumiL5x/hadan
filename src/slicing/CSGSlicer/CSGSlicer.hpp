#ifndef __csg_slicer__
#define __csg_slicer__

#include <slicing/IMeshSlicer.hpp>
#include <maya/MDagPath.h>

class CSGSlicer : public IMeshSlicer {
public:
	CSGSlicer();
	virtual ~CSGSlicer();

	virtual bool setSource( MFnMesh& source ) override;
	virtual bool slice( const Cell& cell, const MeshSlicerInfo& info, MFnMesh& outMesh ) override;

private:
	MDagPath _sourceMesh;
};

#endif /* __csg_slicer__ */