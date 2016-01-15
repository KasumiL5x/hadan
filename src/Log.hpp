#ifndef __log__
#define __log__

#include <string>
#include <maya/MGlobal.h>

namespace Log {
	enum class Dest {
		Std,
		Maya
	};
	
	static const Dest DEFAULT_DEST = Dest::Std;

	static void info( const char* message, Dest destination ) {
		switch( destination ) {
			case Dest::Std: {
				fprintf(stdout, message);
				fflush(stdout);
				break;
			}

			case Dest::Maya: {
				MGlobal::displayInfo(message);
				break;
			}
		}
	}

	static void info( const std::string& message, Dest destination=DEFAULT_DEST ) {
		info(message.c_str(), destination);
	}

	static void warning( const char* message, Dest destination ) {
		switch( destination ) {
			case Dest::Std: {
				fprintf(stdout, message);
				fflush(stdout);
				break;
			}

			case Dest::Maya: {
					
				MGlobal::displayWarning(message);
				break;
			}
		}
	}

	static void warning( const std::string& message, Dest destination=DEFAULT_DEST ) {
		warning(message.c_str(), destination);
	}

	static void error( const char* message, Dest destination ) {
		switch( destination ) {
			case Dest::Std: {
				fprintf(stderr, message);
				fflush(stderr);
				break;
			}

			case Dest::Maya: {
				MGlobal::displayError(message);
				break;
			}
		}
	}

	static void error( const std::string& message, Dest destination=DEFAULT_DEST ) {
		error(message.c_str(), destination);
	}
}

#endif /* __log__ */