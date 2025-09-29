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
class SinglePassFlowFieldScene : public ofxFadeScene {
	
public:
	
	// set the scene name through the base class initializer
	SinglePassFlowFieldScene():ofxFadeScene("SinglePassFlowFieldScene") {};
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
	ofxAutoReloadedShader shaderDraw;
	ofFbo fboBufferA; // with alpha
	ofFbo fboImage; // with alpha
	ofParameter<bool> toggleGuiDraw;
	std::shared_ptr<ActorManager> actorManager;
};
