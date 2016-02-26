#ifndef __syntax__
#define __syntax__

#include <maya/MSyntax.h>

namespace HadanArgs {
	// mesh name
	static const char* HadanMeshName = "-mn";
	static const char* HadanMeshNameLong = "-meshName";
	static const MSyntax::MArgType HadanMeshNameType = MSyntax::kString;

	// fracture type
	static const char* HadanFractureType = "-ft";
	static const char* HadanFractureTypeLong = "-fractureType";
	static const MSyntax::MArgType HadanFractureTypeType = MSyntax::kString;

	// uniform count
	static const char* HadanUniformCount = "-uc";
	static const char* HadanUniformCountLong = "-uniformCount";
	static const MSyntax::MArgType HadanUniformCountType = MSyntax::kUnsigned;

	// primary count
	static const char* HadanPrimaryCount = "-pc";
	static const char* HadanPrimaryCountLong = "-primaryCount";
	static const MSyntax::MArgType HadanPrimaryCountType = MSyntax::kUnsigned;

	// secondary count
	static const char* HadanSecondaryCount = "-sc";
	static const char* HadanSecondaryCountLong = "-secondaryCount";
	static const MSyntax::MArgType HadanSecondaryCountType = MSyntax::kUnsigned;

	// separation distance
	static const char* HadanSeparateDistance = "-sd";
	static const char* HadanSeparateDistanceLong = "-separationDistance";
	static const MSyntax::MArgType HadanSeparateDistanceType = MSyntax::kDouble;

	// sample count
	static const char* HadanSamples = "-sam";
	static const char* HadanSamplesLong = "-sampleCount";
	static const MSyntax::MArgType HadanSamplesType = MSyntax::kUnsigned;

	// fluctuation distance
	static const char* HadanFluxPercentage = "-flp";
	static const char* HadanFluxPercentageLong = "-fluxPercent";
	static const MSyntax::MArgType HadanFluxPercentageType = MSyntax::kDouble;

	// random seed for reproducible results
	static const char* HadanRandomSeed = "-rs";
	static const char* HadanRandomSeedLong = "-randomseed";
	static const MSyntax::MArgType HadanRandomSeedType = MSyntax::kUnsigned;

	// user-provided point
	static const char* HadanPoint = "-pnt";
	static const char* HadanPointLong = "-point";
	static const MSyntax::MArgType HadanPointType = MSyntax::kDouble;

	// smoothing angle
	static const char* HadanSmoothingAngle = "-sa";
	static const char* HadanSmoothingAngleLong = "-smoothingangle";
	static const MSyntax::MArgType HadanSmoothingAngleType = MSyntax::kDouble;

	// multi-threading
	static const char* HadanMultiThreading = "-mt";
	static const char* HadanMultiThreadingLong = "-multithreaded";
	static const MSyntax::MArgType HadanMultiThreadingType = MSyntax::kBoolean;

	static MSyntax Syntax() {
		MSyntax syntax;
		syntax.addFlag(HadanMeshName, HadanMeshNameLong, HadanMeshNameType);
		syntax.addFlag(HadanFractureType, HadanFractureTypeLong, HadanFractureTypeType);
		syntax.addFlag(HadanUniformCount, HadanUniformCountLong, HadanUniformCountType);
		syntax.addFlag(HadanPrimaryCount, HadanPrimaryCountLong, HadanPrimaryCountType);
		syntax.addFlag(HadanSecondaryCount, HadanSecondaryCountLong, HadanSecondaryCountType);
		syntax.addFlag(HadanSeparateDistance, HadanSeparateDistanceLong, HadanSeparateDistanceType);
		syntax.addFlag(HadanSamples, HadanSamplesLong, HadanSamplesType);
		syntax.addFlag(HadanFluxPercentage, HadanFluxPercentageLong, HadanFluxPercentageType);
		syntax.addFlag(HadanRandomSeed, HadanRandomSeedLong, HadanRandomSeedType);
		syntax.addFlag(HadanPoint, HadanPointLong, HadanPointType, HadanPointType, HadanPointType);
		syntax.addFlag(HadanSmoothingAngle, HadanSmoothingAngleLong, HadanSmoothingAngleType);
		syntax.addFlag(HadanMultiThreading, HadanMultiThreadingLong, HadanMultiThreadingType);
		syntax.makeFlagMultiUse(HadanPoint);
		return syntax;
	}
}

#endif /* __syntax__ */