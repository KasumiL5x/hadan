#ifndef __closed_convex_slicer__
#define __closed_convex_slicer__

#include "IMeshSlicer.hpp"

class ClosedConvexSlicer : public IMeshSlicer {
public:
	ClosedConvexSlicer();
	virtual ~ClosedConvexSlicer();

	virtual bool slice( Model& sourceModel, const Cell& cell, Model& outModel ) const override;
};

#endif /* __closed_convex_slicer__ */