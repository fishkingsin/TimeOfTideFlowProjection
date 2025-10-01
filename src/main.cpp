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
	windowSettings.setSize(1920, 1080);
	
	if (DEBUG) {
		windowSettings.windowMode = OF_WINDOW;
		windowSettings.decorated = true;
	} else {
		windowSettings.windowMode = OF_GAME_MODE;
		windowSettings.decorated = false;
	}
	ofCreateWindow(windowSettings);

	ofRunAppWithAppUtils(new ofApp());
}
