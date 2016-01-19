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


	static MSyntax Syntax() {
		MSyntax syntax;
		syntax.addFlag(MeshName, MeshNameLong, MeshNameType);
		syntax.addFlag(SliceCount, SliceCountLong, SliceCountType);
		syntax.addFlag(FractureType, FractureTypeLong, FractureTypeType);
		syntax.addFlag(SeparateDistance, SeparateDistanceLong, SeparateDistanceType);
		return syntax;
	}
}

#endif /* __syntax__ */