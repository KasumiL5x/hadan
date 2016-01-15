#ifndef __mesh_slicer_factory__
#define __mesh_slicer_factory__

#include <memory>
#include "IMeshSlicer.hpp"
#include "ClosedConvexSlicer/ClosedConvexSlicer.hpp"

class MeshSlicerFactory {
public:
	enum class Type {
		ClosedConvex
	};

	static std::unique_ptr<IMeshSlicer> create( Type type ) {
		switch( type ) {
			case Type::ClosedConvex: {
				return std::make_unique<ClosedConvexSlicer>();
			}
		}
		return nullptr;
	}
};

#endif /* __mesh_slicer_factory__ */