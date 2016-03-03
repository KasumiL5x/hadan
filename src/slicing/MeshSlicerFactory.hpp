#ifndef __mesh_slicer_factory__
#define __mesh_slicer_factory__

#include <memory>
#include "IMeshSlicer.hpp"
#include "ClosedConvexSlicer/ClosedConvexSlicer.hpp"
#include "CSGSlicer/CSGSlicer.hpp"

class MeshSlicerFactory {
public:
	enum class Type {
		GTE,  /**< Geometry Tools Engine's ClipMesh */
		CSGJS /**< csg.js boolean slicer */
	};

	static std::unique_ptr<IMeshSlicer> create( Type type ) {
		switch( type ) {
			case Type::GTE: {
				return std::make_unique<ClosedConvexSlicer>();
			}

			case Type::CSGJS: {
				return std::make_unique<CSGSlicer>();
			}
		}
		return nullptr;
	}
};

#endif /* __mesh_slicer_factory__ */