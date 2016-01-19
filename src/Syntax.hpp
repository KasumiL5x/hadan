#ifndef __syntax__
#define __syntax__

#include <maya/MSyntax.h>

namespace HadanArgs {
	// mesh name
	static const char* MeshName = "-mn";
	static const char* MeshNameLong = "-meshname";
	static const MSyntax::MArgType MeshNameType = MSyntax::kString;

	// slice count
	static const char* SliceCount = "-sc";
	static const char* SliceCountLong = "-slicecount";
	static const MSyntax::MArgType SliceCountType = MSyntax::kUnsigned;

	// fracture type
	static const char* FractureType = "-ft";
	static const char* FractureTypeLong = "-fracturetype";
	static const MSyntax::MArgType FractureTypeType = MSyntax::kString;

	// separation distance
	static const char* SeparateDistance = "-sd";
	static const char* SeparateDistanceLong = "-separationdistance";
	static const MSyntax::MArgType SeparateDistanceType = MSyntax::kDouble;

	// user-provided point
	static const char* Point = "-pnt";
	static const char* PointLong = "-point";
	static const MSyntax::MArgType PointType = MSyntax::MArgType::kDouble;

	static MSyntax Syntax() {
		MSyntax syntax;
		// mesh name
		syntax.addFlag(MeshName, MeshNameLong, MeshNameType);
		// slice count
		syntax.addFlag(SliceCount, SliceCountLong, SliceCountType);
		// fracture type
		syntax.addFlag(FractureType, FractureTypeLong, FractureTypeType);
		// separate distance
		syntax.addFlag(SeparateDistance, SeparateDistanceLong, SeparateDistanceType);
		// point (x, y, z; can be repeated)
		syntax.addFlag(Point, PointLong, PointType, PointType, PointType);
		syntax.makeFlagMultiUse(Point);
		return syntax;
	}
}

#endif /* __syntax__ */