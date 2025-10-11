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
	: ofxFadeScene("SinglePassFlowFieldScene")
	, actorManager(actorManager_) {
	setSingleSetup(true); // call setup each time the scene is loaded
	setFade(5000, 5000); // 1 second fade in/out
}

// scene setup
void SinglePassFlowFieldScene::setup() {
	ofEnableAlphaBlending();
	ofSetLogLevel(OF_LOG_VERBOSE);

	shaderA.load("shaders/singlePassFlowField/bufferA");
	shaderB.load("shaders/singlePassFlowField/bufferB");
	shaderDraw.load("shaders/singlePassFlowField/image");

	densityWidth = ofGetWidth();
	densityHeight = ofGetHeight();
	// process all but the density on 16th resolution
	simulationWidth = densityWidth / 2;
	simulationHeight = densityHeight / 2;
	windowWidth = ofGetWindowWidth();
	windowHeight = ofGetWindowHeight();

	fboBufferA.allocate(densityWidth, densityHeight, GL_RGBA32F_ARB);
	fboBufferB.allocate(densityWidth, densityHeight, GL_RGBA32F_ARB);
	fboImage.allocate(densityWidth, densityHeight, GL_RGBA32F_ARB);

	gui.setup();
	gui.add(toggleGuiDraw.set("debug", false));

	// Flow field parameters
	flowFieldParams.setName("flowField");
	flowFieldParams.add(itersParam.set("iters", 3, 1, 8));
	flowFieldParams.add(smpDstParam.set("smpDst", 2, 0, 4));
	flowFieldParams.add(tScaleParam.set("tScale", 0.275, 0.0, 2.0));
	flowFieldParams.add(scaleParam.set("scale", 3.5, 0.1, 10.0));
	flowFieldParams.add(pVelParam.set("pVel", 0.6, 0.0, 3.0));
	flowFieldParams.add(decayParam.set("decay", 0.001, 0.0, 1));
	flowFieldParams.add(spawnRateParam.set("spawnRate", 5, 0, 1000));
	flowFieldParams.add(birthRateParam.set("birthRateParam", 100, 0, 1600));
	flowFieldParams.add(impulseParam.set("impulse", 7.0, 0.0, 20.0));
	flowFieldParams.add(force.set("force", ofVec3f::zero(), ofVec3f(-1, -1), ofVec3f(1, 1)));
	flowFieldParams.add(speed.set("speed", 1, 0, 10));
	flowFieldParams.add(brightness.set("brightness", 0.5, 0, 1));
	gui.add(flowFieldParams);

	
	positionsGroup.setName("positions");
	for (int i = 0; i < MAX_POS; i++) {
		positionsGroup.add(positionsParameter[i]
				.set(
					"position" + ofToString(i)
					// random init position
					,
					ofVec4f(0, ofRandom(ofGetHeight()), 0, impulseParam),
					ofVec4f::zero(),
					ofVec4f(ofGetWidth(), ofGetHeight(), 1, 5)
					 ));
	}
	gui.add(positionsGroup);
	
	additionalPositionsGroup.setName("additional_positions");
	for (int i = 0; i < MAX_ADD_POS; i++) {
		additionalPositionsGroup.add(additionalPositionsParameter[i]
				.set(
					"position" + ofToString(i)
					// random init position
					,
					 ofVec4f(0, ofRandom(ofGetHeight()), 0, impulseParam),
					 ofVec4f::zero(),
					 ofVec4f(ofGetWidth(), ofGetHeight(), 1, 5)
					 ));
	}
	gui.add(additionalPositionsGroup);
	
	minimizeGui(&gui);

	if (!ofFile("SinglePassFlowFieldScene-settings.xml")) {
		gui.saveToFile("SinglePassFlowFieldScene-settings.xml");
	}
	gui.loadFromFile("SinglePassFlowFieldScene-settings.xml");

	minimizeGui(&gui);
}

//--------------------------------------------------------------
void SinglePassFlowFieldScene::minimizeGui(ofxGuiGroup * _group) {
	for (int i = 0; i < _group->getNumControls(); i++) {
		ofxGuiGroup * subGroup = dynamic_cast<ofxGuiGroup *>(_group->getControl(i));
		if (subGroup) {
			minimizeGui(subGroup);
			_group->minimizeAll();
		}
	}
}

// called when scene is entering, this is just a demo and this
// implementation is not required for this class
void SinglePassFlowFieldScene::updateEnter() {
	// called on first enter update
	if (isEnteringFirst()) {
		ofLogNotice("SinglePassFlowFieldScene") << "update enter";
		addActorSceneEventListener(actorManager);
	}

	// fade scene calculates normalized alpha value for us
	ofxFadeScene::updateEnter();

	// finished entering?
	if (!isEntering()) {
		ofLogNotice("SinglePassFlowFieldScene") << "update enter done";
	}
}

// normal update
void SinglePassFlowFieldScene::update() {
	for (int i = 0; i < MAX_POS + MAX_ADD_POS; i++) {
		if (i < MAX_POS) {
			positions[i] = positionsParameter[i].get();
		} else {
			positions[i] = additionalPositionsParameter[i - MAX_POS].get();
		}
		
	}
	
	
	
	frame += 1;
	float dt = 1.0 / max(ofGetFrameRate(), 1.f); // more smooth as 'real' deltaTime.

	// apply noiseshader to fbo
	if(fboBufferA.isAllocated()) {
		fboBufferA.begin();
		shaderA.begin();
		shaderA.setUniform3f("iResolution", densityWidth, densityHeight, 0);
		shaderA.setUniform1f("texCoordWidthScale", densityWidth);
		shaderA.setUniform1f("texCoordHeightScale", densityHeight);
		shaderA.setUniform1f("iTime", ofGetElapsedTimef());
		shaderA.setUniform1f("iTimeDelta", dt);
		shaderA.setUniform4f("iMouse", ofGetMouseX(), ofGetMouseY(), ofGetMousePressed(), 1.0);
		shaderA.setUniform1i("iFrame", frame);
		shaderA.setUniformTexture("iChannel0", fboBufferA.getTexture(), 1);
		shaderA.setUniform4fv("positions", &positions[0].x, MAX_POS + MAX_ADD_POS);
		
		// bind flow field uniforms
		shaderA.setUniform1i("iters", itersParam);
		shaderA.setUniform1i("smpDst", smpDstParam);
		shaderA.setUniform1f("tScale", tScaleParam);
		shaderA.setUniform1f("scale", scaleParam);
		shaderA.setUniform1f("pVel", pVelParam);
		shaderA.setUniform1f("decay", decayParam);
		shaderA.setUniform1f("spawnRate", spawnRateParam * 0.00001);
		shaderA.setUniform1f("impulse", impulseParam);
		shaderA.setUniform1i("birthRate", birthRateParam);
		shaderA.setUniform2f("force", force.get());
		shaderA.setUniform1f("speed", speed.get());
		
		ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
		shaderA.end();
		fboBufferA.end();
	}
	if(fboBufferB.isAllocated()) {
		fboBufferB.begin();
		shaderB.begin();
		shaderB.setUniform3f("iResolution", densityWidth, densityHeight, 0);
		shaderB.setUniform1f("texCoordWidthScale", densityWidth);
		shaderB.setUniform1f("texCoordHeightScale", densityHeight);
		shaderB.setUniform1f("iTime", ofGetElapsedTimef());
		shaderB.setUniform1i("iFrame", frame);
		shaderB.setUniformTexture("iChannel0", fboBufferA.getTexture(), 1);
		shaderB.setUniform1f("brightness", brightness.get());
		ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
		shaderB.end();
		fboBufferB.end();
	}

	if(fboImage.isAllocated()) {
		fboImage.begin();
		shaderDraw.begin();
		shaderDraw.setUniform3f("iResolution", densityWidth, densityHeight, 0);
		shaderDraw.setUniform1f("texCoordWidthScale", densityWidth);
		shaderDraw.setUniform1f("texCoordHeightScale", densityHeight);
		shaderDraw.setUniform1f("iTime", ofGetElapsedTimef());
		shaderDraw.setUniform1i("iFrame", frame);
		shaderDraw.setUniformTexture("iChannel0", fboBufferB.getTexture(), 1);
		ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
		shaderDraw.end();
		fboImage.end();
	}
}

// called when scene is exiting, this is just a demo and this
// implementation is not required for this class
void SinglePassFlowFieldScene::updateExit() {

	// called on first exit update
	if (isExitingFirst()) {
		ofLogNotice("SinglePassFlowFieldScene") << "update exit";
	}

	// fade scene calculates normalized alpha value for us
	ofxFadeScene::updateExit();

	// finished exiting?
	if (!isExiting()) {
		ofLogNotice("SinglePassFlowFieldScene") << "update exit done";
		removeActorSceneEventListener(actorManager);
	}
}

// draw
void SinglePassFlowFieldScene::draw() {
	ofEnableAlphaBlending();
	ofPushStyle();

	ofSetColor(255, 255, 255, 255 * alpha);

	fboImage.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	ofPopStyle();

	ofDisableAlphaBlending();

	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(0, 0, 0, 255 * (1.0 - alpha));
	ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	ofDisableAlphaBlending();
	ofPopStyle();

	if (toggleGuiDraw) {

		int width = windowWidth * 0.25;
		int height = windowHeight * 0.25;
		fboBufferA.draw(windowWidth - width, 0, width, height);
		ofPushStyle();
		ofSetColor(255, 0, 0);
		for (int i = 0; i < MAX_POS + MAX_ADD_POS; i++) {
			ofDrawCircle(
				positions[i].x * (densityWidth / ofGetWindowWidth()),
				positions[i].y * (densityHeight / ofGetWindowHeight()), 5);
		}
		
		ofPopStyle();
		gui.draw();
	}
}

// cleanup
void SinglePassFlowFieldScene::exit() {
}

void SinglePassFlowFieldScene::onActorSceneEvent(ActorSceneEventArgs & args) {
	if (args.eventType == ActorSceneEventType::Move) {
		// TODO: Handle actor scene event (enter, move, leave) // print rich info
		int index = args.actor->index;
		if (index >= 0 || index < 25) {
			ofLog() << "SinglePassFlowFieldScene::onActorSceneEvent index " << index;
			positionsParameter[index].set(args.actor->getPosition());
		}
		ofLog() << "SinglePassFlowFieldScene::onActorSceneEvent " << args.actor->id << " eventType " << args.eventType << " actor key: " << args.actorEventArgs.key << " position: " << args.actor->getPosition().x << " " << args.actor->getPosition().y;
	}
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
	for (const auto & kv : args.parameters) {
		if (kv.first == "param0") frame = static_cast<int>(kv.second);
		if (kv.first == "param1") densityWidth = static_cast<int>(kv.second);
		// Extend mapping as needed
	}
	ofLogNotice() << "[SinglePassFlowFieldScene] Config updated from cue";
}
