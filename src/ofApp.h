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
#include "ActorManager.hpp"
#include "CueReceiver.h"
#include "CueEventArgs.h"
#include "FlowFieldParticlesScene.hpp"
class FlowToolsScene;

class ofApp : public ofxApp {
public:
	ofApp() { };
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void onCueEvent(CueEventArgs & args);

	FlowToolsScene * flowToolsScene = nullptr;
	CurlFlowScene * curlFlowScene = nullptr;
	SinglePassFlowFieldScene * singlePassFlowFieldScene = nullptr;
	FlowFieldParticlesScene * flowFieldParticlesScene = nullptr;
	ofxSceneManager sceneManager;
	int lastScene;
	// rendering transformer
	ofxTransformer transformer;

	// Global actor event system
	ztReceiver actorReceiver;
	CueReceiver cueReceiver;
	shared_ptr<ActorManager> actorManagerPtr;

#ifdef HAVE_OFX_GUI
	ofxTransformPanel panel;
#endif
	long sceneChangeTime;
	long lastSceneChangeTime;
	bool isShowingGui();
};
