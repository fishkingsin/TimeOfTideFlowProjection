//
//  SinglePassFlowField.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 21/9/2025.
//

#include "ActorManager.hpp"
#include "CueEventArgs.h"
#include "ofxAppUtils.h"
#include "ofxAutoReloadedShader.h"
#include "ofxGui.h"
#define MAX_POS 25
#define MAX_ADD_POS 5
class SinglePassFlowFieldScene : public ofxFadeScene {

public:
	void onCueConfigEvent(CueEventArgs & args);

	// set the scene name through the base class initializer
	SinglePassFlowFieldScene()
		: ofxFadeScene("SinglePassFlowFieldScene") { };
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

	int frame, densityWidth, densityHeight, simulationWidth, simulationHeight, windowWidth, windowHeight;
	ofxAutoReloadedShader shaderA;
	ofxAutoReloadedShader shaderB;
	ofxAutoReloadedShader shaderDraw;
	ofFbo fboBufferA; // with alpha
	ofFbo fboBufferB; // with alpha
	ofFbo fboImage; // with alpha
	ofParameter<bool> toggleGuiDraw;
	std::shared_ptr<ActorManager> actorManager;
	ofxPanel gui;

	// Flow field shader (bufferA) controls
	ofParameterGroup flowFieldParams;
	ofParameter<int> itersParam, smpDstParam, birthRateParam, spawnRateParam;
	ofParameter<float> tScaleParam, scaleParam, pVelParam, decayParam, impulseParam, speed;
	ofParameter<ofVec2f> force;

	ofParameterGroup positionsGroup;
	ofParameter<ofVec4f> positionsParameter[MAX_POS];
	ofParameterGroup additionalPositionsGroup;
	ofParameter<ofVec4f> additionalPositionsParameter[MAX_ADD_POS];
	
	ofVec4f positions[MAX_POS + MAX_ADD_POS];
	
	void minimizeGui(ofxGuiGroup * _group);
};
