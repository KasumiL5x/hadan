#ifndef __point_gen_factory__
#define __point_gen_factory__

#include <memory>
#include "IPointGen.hpp"
#include "Uniform/UniformPointGen.hpp"
#include "Bezier/BezierPointGen.hpp"
#include "Cluster/ClusterPointGen.hpp"
#include "TestPointGen/TestPointGen.hpp"

class PointGenFactory {
public:
	enum class Type {
		Invalid,
		Uniform,
		Bezier,
		Cluster,
		Test
	};

	static std::unique_ptr<IPointGen> create( Type type ) {
		switch( type ) {
			case Type::Uniform: {
				return std::make_unique<UniformPointGen>();
			}

			case Type::Bezier: {
				return std::make_unique<BezierPointGen>();
			}

			case Type::Cluster: {
				return std::make_unique<ClusterPointGen>();
			}

			case Type::Test: {
				return std::make_unique<TestPointGen>();
			}
		}
		return nullptr;
	}
};

#endif /* __point_gen_factory__ */