#ifndef __imeshslicer__
#define __imeshslicer__

#include <cells/Cell.hpp>
#include <maya/MFnMesh.h>

class IMeshSlicer {
public:
	IMeshSlicer() {
	}
	virtual ~IMeshSlicer() {
	}

	/**
	 * Sets the source geometry from Maya.
	 * @param[in] source MFnMesh source.
	 * @returns True upon success; false otherwise.
	 */
	virtual bool setSource( MFnMesh& source )=0;

	/**
	 * Slices using a Cell.
	 * @param[in]  cell    Cell that controls the slicing region.
	 * @param[out] outMesh Output sliced Maya Mesh.
	 * @returns True upon success; false otherwise.
	 */
	virtual bool slice( const Cell& cell, MFnMesh& outMesh )=0;
};

#endif /* __imeshslicer__ */