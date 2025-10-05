//
//  GPUCurlFlowScene.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 21/9/2025.
//

#include "GPUCurlFlowScene.hpp"

#define MAX_NOISE_SCALE 100000

// set the scene name through the base class initializer
GPUCurlFlowScene::GPUCurlFlowScene() : ofxFadeScene("GPUCurlFlow"){
	setSingleSetup(true); // call setup each time the scene is loaded
	setFade(5000, 5000); // 1 second fade in/out
}

// scene setup
void GPUCurlFlowScene:: setup() {
	width = ofGetWidth();
	height = ofGetHeight();
	
	ofEnableNormalizedTexCoords();
	
	setupCtlGui(10, 10);
	
	//dimensions of the noise texture
	float w = 64.0;
	float h = w;
	
	noiseGen.setup(3, 4.0, 3.0);
	noiseGen.setSigned(false);
	
	noiseImage.allocate(w, h, OF_IMAGE_GRAYSCALE);
	noiseGen.renderToImage(noiseImage);
	
	imgMinMax(noiseImage);
	
	step = 0;
	dt = 0.005;
	
	curl.load("curl");
	
	ofLogLevel ll = ofGetLogLevel();
	ofSetLogLevel(OF_LOG_WARNING);
	curlTarget.allocate(w, h, GL_RGBA32F_ARB);
	curlTarget.getTextureReference().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
	curlTarget.getTextureReference().setTextureWrap(GL_CLAMP, GL_CLAMP);

	ofSetLogLevel(ll);
	
	setupTranformFeedback();

	drawGui = true;
	int densityWidth = ofGetWidth();
	int densityHeight = ofGetHeight();
	
	fboBuffer.allocate(densityWidth, densityHeight, GL_RGBA32F_ARB);
}

// called when scene is entering, this is just a demo and this
// implementation is not required for this class
void GPUCurlFlowScene:: updateEnter() {
	// called on first enter update
	if(isEnteringFirst()) {
		ofLogNotice("GPUCurlFlowScene") << "update enter";
	}

	// fade scene calculates normalized alpha value for us
	ofxFadeScene::updateEnter();
	
	// finished entering?
	if(!isEntering()) {
		ofLogNotice("GPUCurlFlowScene") << "update enter done";
	}
}

// normal update
void GPUCurlFlowScene:: update() {
	//this checks to see if noise params has changed and generates the curl if so
	setupNoise();
	
	step += dt;
	
	
	advect();
	
	
}

// called when scene is exiting, this is just a demo and this
// implementation is not required for this class
void GPUCurlFlowScene:: updateExit() {
	
	// called on first exit update
	if(isExitingFirst()) {
		ofLogNotice("GPUCurlFlowScene") << "update exit";
	}
	
	// fade scene calculates normalized alpha value for us
	ofxFadeScene::updateExit();
	
	// finished exiting?
	if(!isExiting()) {
		ofLogNotice("GPUCurlFlowScene") << "update exit done";
	}
}

// draw
void GPUCurlFlowScene:: draw() {
	ofEnableAlphaBlending();
	ofPushStyle();
	
	ofSetColor(255, 255, 255, 255*alpha);
	
	ofBackground(0);
	
	fboBuffer.begin();
	ofEnableAlphaBlending();
	ofFill();
	ofSetColor(0, 0, 0, overlayAlpha);
	ofDrawRectangle(0,0, ofGetWindowWidth(), ofGetWindowHeight());

	ofSetColor(255);
	
	switch (drawMode) {
		case 1:
			noiseImage.draw(0, 0, ofGetWidth(), ofGetHeight());
			break;
		case 2:
			curlDrawImage.draw(0, 0, ofGetWidth(), ofGetHeight());
			break;
		default:
			break;
	}
	
	glPointSize(1.0);
	glEnable(GL_BLEND);
	
	ofSetColor(254, 255 * drawAlpha);
	glBindVertexArray(vaoVec[curvao]);
	glDrawArrays(GL_POINTS, 0, partcnt);
	glBindVertexArray(0);
	
	glDisable(GL_BLEND);
	ofDisableAlphaBlending();
	fboBuffer.end();
	fboBuffer.draw(0, 0);
	drawGUI();

	ofPopStyle();
	ofDisableAlphaBlending();
	
}

void GPUCurlFlowScene::drawGUI() {
	if (drawGui) {
		ofSetColor(255, 255);
		ofEnableAlphaBlending();
		ctlGui.draw();

		if (noiseDebugView) {
			float dispw, disph;
			disph = dispw = 200.0;
			
			ofPushMatrix();
			
			ofTranslate(ofGetWidth() - dispw, 0);
			noiseImage.draw(0, 0, dispw, disph);
			ofTranslate(0, disph);
			curlTarget.draw(0, 0, dispw, disph);
			ofTranslate(0, disph);
			curlDrawImage.draw(0, 0, dispw, disph);
			
			ofPopMatrix();
		}

		stringstream ss;
		ss << "FPS: " << ofToString(ofGetFrameRate(), 0) << endl
		<< "upd: " << ofToString(update_time, 1) << endl
		<< "parts: " << partcnt << endl
		<< " press m drawMode : " << drawMode << endl
		<< " press g drawGui : " << drawGui << endl
		<< " press f toggle Fullscreen" << endl
		<< endl;

		ofPushStyle();
		ofPushMatrix();
		float boxw = 150.0;
		float boxh = 60.0f;
		ofTranslate(width - boxw, height - boxh);
		ofSetColor(0, 255);
		ofRect(0, 0, boxw, boxh);
		ofSetColor(255);
		ofDrawBitmapString(ss.str(), 10, 20);
		ofPopMatrix();
		ofPopStyle();
	}
}

// cleanup
void GPUCurlFlowScene:: exit() {

}



void GPUCurlFlowScene::genCurl()
{
	curlTarget.begin();
	ofClear(0, 255);
	ofSetColor(255, 255);
	curl.begin();
	curl.setUniformTexture("uPtex", noiseImage.getTextureReference(), 0);
	curl.setUniform2f("dims", noiseImage.getWidth(), noiseImage.getHeight());
	
	ofRect(0, 0, curlTarget.getWidth(), curlTarget.getHeight());
	curl.end();
	
	curlTarget.end();
	
	// this stuff isnt really required, done so I can see the noise/curl
	curlData.allocate(curlTarget.getWidth(), curlTarget.getHeight(), OF_IMAGE_COLOR_ALPHA);
	ofLogLevel ll = ofGetLogLevel();
	ofSetLogLevel(OF_LOG_FATAL_ERROR);
	curlTarget.getTextureReference().readToPixels(curlData.getPixelsRef());
	ofSetLogLevel(ll);
	curlData.update();
	
	cout <<"curl min/max" << endl;
	imgMinMax(curlData);
	
	curlDrawImage.allocate(curlTarget.getWidth(), curlTarget.getHeight(), OF_IMAGE_COLOR_ALPHA);
	curlDrawImage = curlData;
	noiseGen.normalizeImage(curlDrawImage);
}

void GPUCurlFlowScene::advect()
{
	advectShader.begin();
	advectShader.setUniformTexture("Sampler", curlTarget.getTextureReference(), 0);
	advectShader.setUniform1f("Time", step);
	advectShader.setUniform1f("curlamt", curlAmt);
	advectShader.setUniform2f("constVel", constXval, constYval);
	advectShader.setUniform1f("advectDt", advectDt);
	advectShader.setUniform2f("dims", ofGetWidth(), ofGetHeight());
	glBindVertexArray(vaoVec[curvao]);
	
	glEnable(GL_RASTERIZER_DISCARD);
	
	GLuint tgt = curvao == 0 ? ParticleBufferB : ParticleBufferA;
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tgt);
	
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, partcnt);
	glEndTransformFeedback();
	
	glBindVertexArray(0);
	glDisable(GL_RASTERIZER_DISCARD);
	
	advectShader.end();
	
	curvao = (curvao + 1) % 2;
}

void GPUCurlFlowScene::setupTranformFeedback()
{

	particles.resize(partcnt);
	for (int i = 0; i < partcnt; i++)
		particles[i] = ofVec2f(ofRandomWidth(), ofRandomHeight());
	
	advectShader.setupShaderFromFile(GL_VERTEX_SHADER, "advect.vert");
	advectShader.bindDefaults();
	advectProgram = advectShader.getProgram();
	
	const char* varyings[1] = { "vPosition"};
	glTransformFeedbackVaryings(advectProgram, 1, varyings, GL_INTERLEAVED_ATTRIBS);
	
	advectShader.linkProgram();
	
	SlotPosition =  advectShader.getAttributeLocation("Position");

	glGenBuffers(1, &ParticleBufferA);
	glBindBuffer(GL_ARRAY_BUFFER, ParticleBufferA);
	glBufferData(GL_ARRAY_BUFFER, partcnt * sizeof(ofVec2f), &particles[0], GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glGenBuffers(1, &ParticleBufferB);
	glBindBuffer(GL_ARRAY_BUFFER, ParticleBufferB);
	glBufferData(GL_ARRAY_BUFFER, partcnt * sizeof(ofVec2f), 0, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//setup VAOs
	glGenVertexArrays(2, vaoVec);
	
	glBindVertexArray(vaoVec[0]);
	
	glBindBuffer(GL_ARRAY_BUFFER, ParticleBufferA);
	glEnableVertexAttribArray(SlotPosition);
	glVertexAttribPointer(SlotPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ofVec2f), 0);
	
	glBindVertexArray(vaoVec[1]);
	
	glBindBuffer(GL_ARRAY_BUFFER, ParticleBufferB);
	glEnableVertexAttribArray(SlotPosition);
	glVertexAttribPointer(SlotPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ofVec2f), 0);
	
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	curvao = 0;
}


#pragma mark - misc/util -

void GPUCurlFlowScene::imgMinMax(ofFloatImage &img)
{
	ofVec4f minvals(9000.0f);
	ofVec4f maxvals(-9000.0f);
	
	for (int x = 0; x < img.getWidth(); x++) {
		for (int y = 0; y < img.getHeight(); y++) {
			ofFloatColor a = img.getColor(x, y);
			minvals.x = fmin(minvals.x, a.r);
			minvals.y = fmin(minvals.y, a.g);
			minvals.z = fmin(minvals.z, a.b);
			minvals.w = fmin(minvals.w, a.a);
			maxvals.x = fmax(maxvals.x, a.r);
			maxvals.y = fmax(maxvals.y, a.g);
			maxvals.z = fmax(maxvals.z, a.b);
			maxvals.w = fmax(maxvals.w, a.a);

		}
	}
	cout << "minvals: " << minvals << endl;
	cout << "maxvals: " << maxvals << endl;
	
}

#pragma mark - Ctl Gui -

void GPUCurlFlowScene::setupCtlGui(int x, int y)
{
	ctlGui.setup("ctls", ctlGuiFile, x, y);
	ctlGui.add(toggleGuiDraw.set("Togogle GUI", true));
	ctlGui.add(numOctaves.setup("octaves", 1, 1, 8));
	ctlGui.add(noiseAlpha.setup("alpha", 2.0, 0.1, 8.0));
	ctlGui.add(noiseBeta.setup("beta", 2.0, 0.1, 8.0));
	ctlGui.add(noiseSigned.setup("signed", false));
	ctlGui.add(noiseDebugView.setup("noise view", false));
	
	ctlGui.add(constXval.setup("x amt", 0.0, -2.0, 2.0));
	ctlGui.add(constYval.setup("y amt", 0.0, -2.0, 2.0));
	ctlGui.add(curlAmt.setup("curl amt", 0.25, 0.0, 2.0));
	ctlGui.add(advectDt.setup("advect dt", 1.0, 0.0, 2.0));
	ctlGui.add(drawAlpha.setup("draw alpha", 1.0, 0.0, 1.0));
	ctlGui.add(overlayAlpha.setup("overlayAlpha", 1.0, 0.0, 255));
	ctlGui.loadFromFile(ctlGuiFile);
	
	numOctaves.addListener(this, &GPUCurlFlowScene::noiseParamIntChanged);
	noiseAlpha.addListener(this, &GPUCurlFlowScene::noiseParamFloatChanged);
	noiseBeta.addListener(this, &GPUCurlFlowScene::noiseParamFloatChanged);
	noiseSigned.addListener(this, &GPUCurlFlowScene::noiseParamBoolChanged);
	
	noiseDirty = true;
}

void GPUCurlFlowScene::saveCtlGui()
{
	ctlGui.saveToFile(ctlGuiFile);
}

void GPUCurlFlowScene::setupNoise()
{
	if (noiseDirty == false)
		return;
	
	noiseGen.setup(numOctaves, noiseAlpha, noiseBeta);
	noiseGen.setSigned(noiseSigned);
	updateNoise();
	noiseDirty = false;
}

void GPUCurlFlowScene::updateNoise()
{
	noiseGen.renderToImage(noiseImage);
	genCurl();
}


void GPUCurlFlowScene::noiseParamBoolChanged(bool &v) { noiseDirty = true; }
void GPUCurlFlowScene::noiseParamFloatChanged(float &v) { noiseDirty = true; }
void GPUCurlFlowScene::noiseParamIntChanged(int &v) { noiseDirty = true; }
