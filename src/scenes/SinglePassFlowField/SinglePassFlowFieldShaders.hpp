//
//  SinglePassFlowFieldShaders.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 6/10/2025.
//


#ifndef SinglePassFlowFieldShaders_hpp
#define SinglePassFlowFieldShaders_hpp

#include "ofxShadertoy.h"
#include <stdio.h>
#define MAX_POS 25

class SinglePassFlowFieldShaders: public ofxShadertoy {
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

	ofParameterGroup positionsGroup;
	ofParameter<ofVec3f> positionsParameter[MAX_POS];
	ofVec3f positions[MAX_POS];
	
};

#endif /* SinglePassFlowFieldShaders_hpp */
