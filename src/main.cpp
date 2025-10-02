#include "ofApp.h"
#include "ofMain.h"
#include "testApp.h"

#define USE_PROGRAMMABLE_GL
#define DEBUG 0
//========================================================================
int main() {

	ofGLFWWindowSettings windowSettings;
#ifdef USE_PROGRAMMABLE_GL
	windowSettings.setGLVersion(4, 1);
#endif
	
	
	if (DEBUG) {
		windowSettings.setSize(1280, 720);
		windowSettings.windowMode = OF_WINDOW;
		windowSettings.decorated = false;
	} else {
		windowSettings.setSize(1920, 1080);
		windowSettings.windowMode = OF_GAME_MODE;
		windowSettings.decorated = false;
	}
	ofCreateWindow(windowSettings);

	ofRunAppWithAppUtils(new ofApp());
}
