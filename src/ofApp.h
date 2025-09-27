#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "scenes.h"
// optional ofxGUI control panel for transformer,
// uncomment this if you're using ofxGui in this project
#define HAVE_OFX_GUI

#ifdef HAVE_OFX_GUI
	#include "ofxGui.h"
	#include "ofxTransformPanel.h"
#endif

class FlowToolsScene;

class ofApp : public ofxApp {
public:
	ofApp() {};
	void	setup();
	void	update();
	void	draw();
	void	keyPressed(int key);
	
		
	FlowToolsScene* flowToolsScene;
	CurlFlowScene* curlFlowScene;
	ofxSceneManager sceneManager;
	int lastScene;
	// rendering transformer
	ofxTransformer transformer;

// Global actor event system
ztReceiver receiver;
ActorManager actorManager;

#ifdef HAVE_OFX_GUI
ofxTransformPanel panel;
#endif
};
