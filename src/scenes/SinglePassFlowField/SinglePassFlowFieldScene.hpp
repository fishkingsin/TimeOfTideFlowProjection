//
//  SinglePassFlowField.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 21/9/2025.
//


#include "ofxAppUtils.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"
#include "ActorManager.hpp"
#include "CueEventArgs.h"
#include "SinglePassFlowFieldShaders.hpp"
#include "ofxShadertoy.h"
class SinglePassFlowFieldScene : public ofxFadeScene {
	
public:

	void onCueConfigEvent(CueEventArgs & args);

	// set the scene name through the base class initializer
	SinglePassFlowFieldScene():ofxFadeScene("SinglePassFlowField") {};
	SinglePassFlowFieldScene(std::shared_ptr<ActorManager> actorManager_);
	void setup();
	void updateEnter();
	void update();
	void updateExit();
	void draw();
	void exit();
	void drawGUI();
	
	void setupCtlGui(int x, int y);
	void saveCtlGui();
	void setupNoise();
	void updateNoise();
	
	void onActorSceneEvent(ActorSceneEventArgs & args);
	void addActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr);
	void removeActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr);
	void minimizeGui(ofxGuiGroup * _group);
	int frame, densityWidth, densityHeight, simulationWidth, simulationHeight, windowWidth, windowHeight;
	SinglePassFlowFieldShaders singlePassFlowFieldShaders;
	ofParameter<bool> toggleGuiDraw;
	std::shared_ptr<ActorManager> actorManager;
	ofxPanel gui;
};
