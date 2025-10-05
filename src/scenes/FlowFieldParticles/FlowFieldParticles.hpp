//
//  FlowFieldParticles.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 3/10/2025.
//

#ifndef ofxShadertoyExt_hpp
#define ofxShadertoyExt_hpp

#include "ofxShadertoy.h"
#include <stdio.h>
#define MAX_POS 25

class FlowFieldParticles: public ofxShadertoy {
public:
	void setup();
	int frame, densityWidth, densityHeight, simulationWidth, simulationHeight, windowWidth, windowHeight;
	ofTexture fboA, fboB, fboC, fboD;
	bool load(string shaderfilename, Buffer buffer);
	void applyTexture(ofxShadertoy::Buffer buffer);
	void setupAdditionalShaderUniforms(ofShader const & bufShader, Buffer buffer);
	void drawDebug();

	bool reloadShaders();

	void enableWatchFiles();
	void disableWatchFiles();

	void setMillisBetweenFileCheck(int _millis);

	void _update(ofEventArgs & e);

	void setGeometryInputType(GLenum type);
	void setGeometryOutputType(GLenum type);
	void setGeometryOutputCount(int count);
	// Shader control parameters
	ofParameterGroup parameters;
	
private:
	bool bWatchingFiles;
	bool filesChanged();

	bool loadShaderNextFrame;

	std::time_t getLastModified(ofFile & _file);

	int lastTimeCheckMillis;
	int millisBetweenFileCheck;

	string fileAFileName, fileBFileName, fileCFileName, fileDFileName, fileImageFileName;
	ofFile fileA, fileB, fileC, fileD, fileImage;
	vector<std::time_t> fileChangedTimes;
	ofxShadertoy::Buffer changeBuffer;
	string openFile(ofFile file, string fileName);

	ofParameterGroup parametersA;
	ofParameterGroup parametersB;
	ofParameterGroup parametersC;
	ofParameterGroup parametersD;
	ofParameterGroup parametersImage;

	ofParameter<int> vector_mode;
	ofParameter<float> arrowDensity;
	ofParameter<float> arrowLength;
	ofParameter<int> iter1;
	ofParameter<int> iter2;
	ofParameter<float> scaleParam;
	ofParameter<float> velocityX;
	ofParameter<float> velocityY;
	ofParameter<float> mode2Speed;
	ofParameter<float> mode1Detail;
	ofParameter<float> mode1Twist;
	ofParameter<int> showArrows;
	// Mode 1 color parameterization (matches bufferA.frag new uniforms)
	ofParameter<float> mode1Phase; // maps to mode1_phase
	ofParameter<float> mode1RgAmp; // maps to mode1_rg_amp
	ofParameter<float> mode1RgBias; // maps to mode1_rg_bias
	ofParameter<float> mode1BAmp; // maps to mode1_b_amp
	ofParameter<float> mode1BBias; // maps to mode1_b_bias
	
	// buffer B
	ofParameter<float> particleSpeedScale;
	
	// buffer C
	ofParameter<int> smpDst;
	
	// buffer D
	ofParameter<float> damping;
	
	
	// image
	ofParameter<ofVec3f> baseColor;
	ofParameter<ofVec3f> overrideColor;
	ofParameter<float> colorised;
	ofParameter<float> luminance;
	ofParameter<float> tolerance;
	ofParameter<int> debugMode;

	ofParameterGroup positionsGroup;
	ofParameter<ofVec3f> positionsParameter[MAX_POS];
	ofVec3f positions[MAX_POS];
	
};

#endif /* ofxShadertoyExt_hpp */
