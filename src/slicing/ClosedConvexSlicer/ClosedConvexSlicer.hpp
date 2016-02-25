#ifndef __closed_convex_slicer__
#define __closed_convex_slicer__

#include <slicing/IMeshSlicer.hpp>
#include "../../Model.hpp"

class ClosedConvexSlicer : public IMeshSlicer {
public:
	ClosedConvexSlicer();
	virtual ~ClosedConvexSlicer();

	virtual bool setSource( MFnMesh& source ) override;
	virtual bool slice( const Cell& cell, MFnMesh& outMesh ) override;

private:
	Model _inputModel;
};

#endif /* __closed_convex_slicer__ */