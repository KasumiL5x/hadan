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

	/**
	 * Slices a Model using a Cell.
	 * @param[in]  sourceModel Input Model used as a source for slicing.
	 * @param[in]  cell        Cell that controls the slicing region.
	 * @param[out] outModel    Output sliced Model.
	 * @returns True upon success; false otherwise.
	 */
	virtual bool slice( Model& sourceModel, const Cell& cell, Model& outModel ) const=0;
};

#endif /* __imeshslicer__ */