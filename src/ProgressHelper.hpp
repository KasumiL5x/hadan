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
		MProgressWindow::startProgress();
		return true;
	}

	static void begin( int max, const char* text ) {
		MProgressWindow::setProgressStatus(text);
		MProgressWindow::setProgressRange(0, max);
		MProgressWindow::setProgress(0);
	}

	static void advance() {
		MProgressWindow::advanceProgress(1);
	}

	static void end() {
		MProgressWindow::endProgress();
	}
}

#endif /* __progress_helper__ */