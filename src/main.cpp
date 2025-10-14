#include "ofApp.h"
#include "ofMain.h"
#include "testApp.h"

#define USE_PROGRAMMABLE_GL
#define DEBUG 1
//========================================================================
int main() {

	ofGLFWWindowSettings windowSettings;
#ifdef USE_PROGRAMMABLE_GL
	windowSettings.setGLVersion(4, 1);
#endif

	if (DEBUG) {
		windowSettings.setSize(1920, 1280);
		windowSettings.windowMode = OF_WINDOW;
	} else {
		windowSettings.setSize(1920, 1080);
		windowSettings.windowMode = OF_FULLSCREEN;
		windowSettings.decorated = false;
	}
	ofCreateWindow(windowSettings);

	ofRunAppWithAppUtils(new ofApp());
}
