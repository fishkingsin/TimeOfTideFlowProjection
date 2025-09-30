//
//  SinglePassFlowFieldScene.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 21/9/2025.
//

#include "SinglePassFlowFieldScene.hpp"

#define MAX_NOISE_SCALE 100000

// set the scene name through the base class initializer
SinglePassFlowFieldScene::SinglePassFlowFieldScene(std::shared_ptr<ActorManager> actorManager_)
: ofxFadeScene("GPUCurlFlow")
, actorManager(actorManager_) {
	setSingleSetup(true); // call setup each time the scene is loaded
	setFade(5000, 5000); // 1 second fade in/out
}

// scene setup
void SinglePassFlowFieldScene:: setup() {
	ofDisableArbTex();
	ofEnableAlphaBlending();
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	
	
	shaderA.load("singlePassFlowFieldShaders/bufferA");
	shaderDraw.load("singlePassFlowFieldShaders/image");
	
	
	
	densityWidth = ofGetWidth();
	densityHeight = ofGetHeight();
	// process all but the density on 16th resolution
	simulationWidth = densityWidth / 2;
	simulationHeight = densityHeight / 2;
	windowWidth = ofGetWindowWidth();
	windowHeight = ofGetWindowHeight();
	
	fboBufferA.allocate(densityWidth, densityHeight, GL_RGBA32F_ARB);
	fboImage.allocate(densityWidth, densityHeight, GL_RGBA32F_ARB);
}

// called when scene is entering, this is just a demo and this
// implementation is not required for this class
void SinglePassFlowFieldScene:: updateEnter() {
	// called on first enter update
	if(isEnteringFirst()) {
		ofLogNotice("SinglePassFlowFieldScene") << "update enter";
	}

	// fade scene calculates normalized alpha value for us
	ofxFadeScene::updateEnter();
	
	// finished entering?
	if(!isEntering()) {
		ofLogNotice("SinglePassFlowFieldScene") << "update enter done";
	}
}

// normal update
void SinglePassFlowFieldScene:: update() {
	frame += 1 ;
	float dt = 1.0 / max(ofGetFrameRate(), 1.f); // more smooth as 'real' deltaTime.
	
	// apply noiseshader to fbo
	fboBufferA.begin();
	shaderA.begin();
	shaderA.setUniform3f("iResolution", densityWidth, densityHeight, 0);
	shaderA.setUniform1f("texCoordWidthScale", densityWidth);
	shaderA.setUniform1f("texCoordHeightScale", densityHeight);
	shaderA.setUniform1f("iTime", ofGetElapsedTimef());
	shaderA.setUniform1f("iTimeDelta", dt);
	shaderA.setUniform4f("iMouse", ofGetMouseX(), ofGetMouseY(), ofGetMousePressed(), 1.0);
	shaderA.setUniform1i("iFrame", frame);
	shaderA.setUniformTexture("iChannel0", fboBufferA.getTexture() , 1);
	fboImage.draw(0 ,0);
	shaderA.end();
	fboBufferA.end();
	
	fboImage.begin();
	shaderDraw.begin();
	shaderDraw.setUniform3f("iResolution", densityWidth, densityHeight, 0);
	shaderDraw.setUniform1f("texCoordWidthScale", densityWidth);
	shaderDraw.setUniform1f("texCoordHeightScale", densityHeight);
	shaderDraw.setUniform1f("iTime", ofGetElapsedTimef());
	shaderDraw.setUniform1i("iFrame", frame);
	shaderDraw.setUniformTexture("iChannel0", fboBufferA.getTexture() , 1 );
	
	fboBufferA.draw(0, 0);
	shaderDraw.end();
	fboImage.end();
	
	
}

// called when scene is exiting, this is just a demo and this
// implementation is not required for this class
void SinglePassFlowFieldScene:: updateExit() {
	
	// called on first exit update
	if(isExitingFirst()) {
		ofLogNotice("SinglePassFlowFieldScene") << "update exit";
	}
	
	// fade scene calculates normalized alpha value for us
	ofxFadeScene::updateExit();
	
	// finished exiting?
	if(!isExiting()) {
		ofLogNotice("SinglePassFlowFieldScene") << "update exit done";
	}
}

// draw
void SinglePassFlowFieldScene:: draw() {
	ofEnableAlphaBlending();
	ofPushStyle();
	
	ofSetColor(255, 255, 255, 255*alpha);
	
	fboImage.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	
	if (toggleGuiDraw) {
		int width = windowWidth * 0.25;
		int height = windowHeight * 0.25;
		fboBufferA.draw(0, 0, width, height);
	}

	ofPopStyle();
	ofDisableAlphaBlending();
	
}


// cleanup
void SinglePassFlowFieldScene:: exit() {
	
}

void SinglePassFlowFieldScene::onActorSceneEvent(ActorSceneEventArgs & args) {
	// TODO: Handle actor scene event (enter, move, leave) // print rich info 
	ofLog() << "SinglePassFlowFieldScene::onActorSceneEvent " << args.eventType << " actor key: " << args.actorEventArgs.key << " position: " << args.actor->position;
}

void SinglePassFlowFieldScene::addActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr) {
	ofAddListener(managerPtr->sceneActorEvent, this, &SinglePassFlowFieldScene::onActorSceneEvent);
}

void SinglePassFlowFieldScene::removeActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr) {
ofRemoveListener(managerPtr->sceneActorEvent, this, &SinglePassFlowFieldScene::onActorSceneEvent);
}

void SinglePassFlowFieldScene::onCueConfigEvent(CueEventArgs & args) {
    if (args.cueType != CueType::ConfigUpdate || args.sceneId != 3) return;
    // Example: apply param0 to frame count, param1 to densityWidth
    for (const auto& kv : args.parameters) {
        if (kv.first == "param0") frame = static_cast<int>(kv.second);
        if (kv.first == "param1") densityWidth = static_cast<int>(kv.second);
        // Extend mapping as needed
    }
    ofLogNotice() << "[SinglePassFlowFieldScene] Config updated from cue";
}
