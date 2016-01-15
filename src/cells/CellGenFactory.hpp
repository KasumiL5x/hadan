#ifndef __cell_generator_factory__
#define __cell_generator_factory__

#include <memory>
#include "ICellGen.hpp"
#include "VoronoiCelGen/VoronoiCellGen.hpp"

class CellGenFactory {
public:
	enum class Type {
		Voronoi
	};

	static std::unique_ptr<ICellGen> create( Type type ) {
		switch( type ) {
			case Type::Voronoi: {
				return std::make_unique<VoronoiCellGen>();
			}
		}
		return nullptr;
	}
};

#endif /* __cell_generator_factory__ */