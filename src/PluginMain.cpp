#include <maya/MFnPlugin.h>
#include "Hadan.hpp"
#include "Syntax.hpp"

void* HadanCreator() {
	return static_cast<void*>(new Hadan);
}

MStatus initializePlugin( MObject obj ) {
	MFnPlugin plugin(obj, "KasumiL5x", "0.1-dev", "Any");
	const MStatus status = plugin.registerCommand("hadan", HadanCreator, HadanSyntax);
	if( status != MS::kSuccess ) {
		status.perror("registerCommand");
		return status;
	}
	return status;
}

MStatus uninitializePlugin( MObject obj ) {
	MFnPlugin plugin(obj);
	const MStatus status = plugin.deregisterCommand("hadan");
	if( status != MS::kSuccess ) {
		status.perror("deregisterCommand");
		return status;
	}
	return status;
}