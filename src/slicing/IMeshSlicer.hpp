#ifndef __imeshslicer__
#define __imeshslicer__

#include <cells/Cell.hpp>
#include <Model.hpp>

class IMeshSlicer {
public:
	IMeshSlicer() {
	}
	virtual ~IMeshSlicer() {
	}

	virtual bool slice( Model& sourceModel, const Cell& cell, Model& outModel ) const=0;
};

#endif /* __imeshslicer__ */