#ifndef __syntax__
#define __syntax__

#include <maya/MSyntax.h>

// mesh name
const char* MeshName = "-mn";
const char* MeshNameLong = "-meshname";
const MSyntax::MArgType MeshNameType = MSyntax::kString;

// slice count
const char* SliceCount = "-sc";
const char* SliceCountLong = "-slicecount";
const MSyntax::MArgType SliceCountType = MSyntax::kDouble;

// separation distance
const char* SeparateDistance = "-sd";
const char* SeparateDistanceLong = "-separationdistance";
const MSyntax::MArgType SeparateDistanceType = MSyntax::kDouble;

MSyntax HadanSyntax() {
	MSyntax syntax;
	syntax.addFlag(MeshName, MeshNameLong, MeshNameType);
	syntax.addFlag(SliceCount, SliceCountLong, SliceCountType);
	syntax.addFlag(SeparateDistance, SeparateDistanceLong, SeparateDistanceType);
	return syntax;
}

#endif /* __syntax__ */