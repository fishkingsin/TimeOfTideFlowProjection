//
//  SinglePassFlowField.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 21/9/2025.
//


#include "ofxAppUtils.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"
class SinglePassFlowFieldScene : public ofxFadeScene {
	
public:
	
	// set the scene name through the base class initializer
	SinglePassFlowFieldScene();
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
	int frame, densityWidth, densityHeight, simulationWidth, simulationHeight, windowWidth, windowHeight;
	ofxAutoReloadedShader shaderA;
	ofxAutoReloadedShader shaderDraw;
	ofFbo fboBufferA; // with alpha
	ofFbo fboImage; // with alpha
	bool showGUI = false;
};
