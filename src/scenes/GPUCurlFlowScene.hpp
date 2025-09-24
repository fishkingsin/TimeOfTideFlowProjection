//
//  GPUCurlFlowScene.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 21/9/2025.
//


#include "ofxAppUtils.h"
#include "ofxGui.h"
#include "OctaveNoise.h"
class GPUCurlFlowScene : public ofxFadeScene {
	
public:
	
	// set the scene name through the base class initializer
	GPUCurlFlowScene();
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
	
	ofShader curl;
	
	void genCurl();
	
	void setupTranformFeedback();
	
	void advect();
	
	void imgMinMax(ofFloatImage &img);
	
	ofxPanel ctlGui;
	string ctlGuiFile = "gui.ctls.xml";
	ofxIntSlider numOctaves;
	ofxFloatSlider noiseAlpha;
	ofxFloatSlider noiseBeta;
	ofxToggle noiseSigned;
	ofxToggle noiseDebugView;
	ofxFloatSlider constXval;
	ofxFloatSlider constYval;
	ofxFloatSlider curlAmt;
	ofxFloatSlider advectDt;
	ofxFloatSlider drawAlpha;
	ofParameter<bool>	toggleGuiDraw;
	
	void noiseParamIntChanged(int & v);
	void noiseParamFloatChanged(float & v);
	void noiseParamBoolChanged(bool & v);
	bool noiseDirty;
	
	ofxIntSlider overlayAlpha;
	ofFbo fboBuffer;
	
	
	OctaveNoise noiseGen;
	ofFloatImage noiseImage;
	ofFbo curlTarget;
	ofFloatImage curlDrawImage;
	ofFloatImage curlData;
	
	
	float step;
	float dt;
	
	ofShader advectShader;
	
	GLuint vaoVec[2];
	GLuint advectProgram;
	
	GLuint ParticleBufferB;
	GLuint ParticleBufferA;
	
	GLuint SlotPosition;
	u_int curvao;
	
	
	int partcnt = 1000000;
	vector<ofVec2f> particles;
	
	float update_time;
	float width, height;
	
	int drawMode = 0;
	int drawModeMax = 3;
	
	bool drawGui;
};
