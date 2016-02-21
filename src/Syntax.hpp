#ifndef __syntax__
#define __syntax__

#include <maya/MSyntax.h>

namespace HadanArgs {
	// mesh name
	static const char* MeshName = "-mn";
	static const char* MeshNameLong = "-meshName";
	static const MSyntax::MArgType MeshNameType = MSyntax::kString;

	// fracture type
	static const char* FractureType = "-ft";
	static const char* FractureTypeLong = "-fractureType";
	static const MSyntax::MArgType FractureTypeType = MSyntax::kString;

	// uniform count
	static const char* UniformCount = "-uc";
	static const char* UniformCountLong = "-uniformCount";
	static const MSyntax::MArgType UniformCountType = MSyntax::kUnsigned;

	// primary count
	static const char* PrimaryCount = "-pc";
	static const char* PrimaryCountLong = "-primaryCount";
	static const MSyntax::MArgType PrimaryCountType = MSyntax::kUnsigned;

	// secondary count
	static const char* SecondaryCount = "-sc";
	static const char* SecondaryCountLong = "-secondaryCount";
	static const MSyntax::MArgType SecondaryCountType = MSyntax::kUnsigned;

	// separation distance
	static const char* SeparateDistance = "-sd";
	static const char* SeparateDistanceLong = "-separationDistance";
	static const MSyntax::MArgType SeparateDistanceType = MSyntax::kDouble;

	// sample count
	static const char* Samples = "-sam";
	static const char* SamplesLong = "-sampleCount";
	static const MSyntax::MArgType SamplesType = MSyntax::kUnsigned;

	// fluctuation distance
	static const char* FluxPercentage = "-flp";
	static const char* FluxPercentageLong = "-fluxPercent";
	static const MSyntax::MArgType FluxPercentageType = MSyntax::kDouble;

	// random seed for reproducible results
	static const char* HadanRandomSeed = "-rs";
	static const char* HadanRandomSeedLong = "-randomseed";
	static const MSyntax::MArgType HadanRandomSeedType = MSyntax::kUnsigned;

	// user-provided point
	static const char* Point = "-pnt";
	static const char* PointLong = "-point";
	static const MSyntax::MArgType PointType = MSyntax::kDouble;

	static MSyntax Syntax() {
		MSyntax syntax;
		syntax.addFlag(MeshName, MeshNameLong, MeshNameType);
		syntax.addFlag(FractureType, FractureTypeLong, FractureTypeType);
		syntax.addFlag(UniformCount, UniformCountLong, UniformCountType);
		syntax.addFlag(PrimaryCount, PrimaryCountLong, PrimaryCountType);
		syntax.addFlag(SecondaryCount, SecondaryCountLong, SecondaryCountType);
		syntax.addFlag(SeparateDistance, SeparateDistanceLong, SeparateDistanceType);
		syntax.addFlag(Samples, SamplesLong, SamplesType);
		syntax.addFlag(FluxPercentage, FluxPercentageLong, FluxPercentageType);
		syntax.addFlag(HadanRandomSeed, HadanRandomSeedLong, HadanRandomSeedType);
		syntax.addFlag(Point, PointLong, PointType, PointType, PointType);
		syntax.makeFlagMultiUse(Point);
		return syntax;
	}
}

#endif /* __syntax__ */