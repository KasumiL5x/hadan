#ifndef __progress_helper__
#define __progress_helper__

#include <maya/MProgressWindow.h>

namespace ProgressHelper {
	static int TOTAL_STEPS = 5;

	static bool init() {
		if( !MProgressWindow::reserve() ) {
			return false;
		}
		MProgressWindow::setTitle("Hadan");
		MProgressWindow::setInterruptable(false);
		return true;
	}

	static void setRange( int min, int max ) {
		MProgressWindow::setProgressRange(min, max);
		MProgressWindow::setProgress(min);
	}

	static void start() {
		MProgressWindow::startProgress();
	}

	static void setStatus( const char* text ) {
		MProgressWindow::setProgressStatus(text);
	}

	static void setProgress( int val ) {
		MProgressWindow::setProgress(val);
	}

	static void end() {
		MProgressWindow::endProgress();
	}
}

#endif /* __progress_helper__ */