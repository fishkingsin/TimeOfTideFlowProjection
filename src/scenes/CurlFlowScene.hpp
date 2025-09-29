//
//  CurlFlowScene.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 21/9/2025.
//

#include "ofxAppUtils.h"
#include "ofxGui.h"
#include "ActorManager.hpp"
class CurlFlowScene : public ofxFadeScene {

public:
	// set the scene name through the base class initializer
	CurlFlowScene(std::shared_ptr<ActorManager> _actorManager);
	CurlFlowScene():ofxFadeScene("CurlFlow") {};
	void setup();
	void updateEnter();
	void update();
	void updateExit();
	void draw();
	void exit();
	void onActorSceneEvent(ActorSceneEventArgs & args);
	void addActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr);
	void removeActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr);

	// --- Curl Noise Port Members ---
	struct Disc {
		ofVec2f pos;
		ofVec2f vel;
		float radius;
		ofColor color;
	};

	std::vector<Disc> discs;
	std::vector<int> red, grn, blu;

	std::shared_ptr<ActorManager> actorManager;

	int discCount;
	bool flow;

	// Color generation parameters
	float center, width, frequency1, frequency2, frequency3;
	float phase1, phase2, phase3;

	// Variables for controls
	float speed;
	float fade;
	float step;
	float particle_size;
	bool rainbow;
	ofColor baseColor;

	// GUI
	ofxPanel gui;
	ofParameter<bool> toggleGuiDraw;
	ofParameter<int> guiFPS;
	ofParameter<int> guiDiscCount;
	ofParameter<bool> guiFlow;
	ofParameter<float> noiseScale;
	ofParameter<float> guiCenter, guiWidth;
	ofParameter<float> guiFrequency1, guiFrequency2, guiFrequency3;
	ofParameter<float> guiPhase1, guiPhase2, guiPhase3;
	ofParameter<float> guiSpeed, guiFade, guiStep, guiParticleSize, xVol, yVol;
	ofParameter<bool> guiRainbow;
	ofParameter<ofColor> guiBaseColor;

	// Utility
	void move();
	ofVec2f computeCurl(float x, float y);
	void reset();
	void clearBackground();
	void randomize();

	//32 bits red, 32 bits green, 32 bits blue, from 0 to 1 in 'infinite' steps
	ofFbo rgbaFboFloat; // with alpha
};
