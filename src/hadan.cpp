#include <maya/MIOStream.h>
#include <maya/MSimple.h>

DeclareSimpleCommand(hadan, "KasumiL5x", "0.0.1-dev");

MStatus hadan::doIt( const MArgList& args ) {
	std::cout << "Hello, " << args.asString(0).asChar() << std::endl;
	return MStatus::kSuccess;
}