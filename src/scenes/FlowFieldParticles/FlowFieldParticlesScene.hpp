//
//  FlowFieldParticleScene.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 3/10/2025.
//
#include "ofMain.h"
#include "ofxAppUtils.h"
#include "ofxGui.h"
#include "ofxFlowTools.h"
#include "ftActorFlow.hpp"
#include "ztReceiver.hpp"
#include "ActorManager.hpp"
#include "ofxShadertoy.h"
#include "CueEventArgs.h"
#include "FlowFieldParticles.hpp"
class FlowFieldParticlesScene: public ofxFadeScene {
public:
	FlowFieldParticlesScene(std::shared_ptr<ActorManager> _actorManager);
	FlowFieldParticlesScene():ofxFadeScene("FlowFieldParticles") {};
	void setup();
	void updateEnter();
	void update();
	void updateExit();
	void draw();
	void exit();
	void onCueConfigEvent(CueEventArgs & args);
	void onActorSceneEvent(ActorSceneEventArgs & args);
	void addActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr);
	void removeActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr);
	void minimizeGui(ofxGuiGroup * _group);
	std::shared_ptr<ActorManager> actorManager;
	ofxPanel gui;
	ofParameter<bool> toggleGuiDraw;
	ofParameter<int> guiFPS;

	FlowFieldParticles flowFieldParticlesShaders;
	int frame, densityWidth, densityHeight, simulationWidth, simulationHeight, windowWidth, windowHeight;
};
