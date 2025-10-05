//
//  FlowFieldParticlesScene.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 3/10/2025.
//

#include "FlowFieldParticlesScene.hpp"


FlowFieldParticlesScene::FlowFieldParticlesScene(std::shared_ptr<ActorManager> _actorManager)
	: ofxFadeScene("FlowFieldParticles")
	, actorManager(_actorManager) {
	setSingleSetup(true); // call setup each time the scene is loaded
	setFade(5000, 5000); // 1 second fade in/out
}

void FlowFieldParticlesScene::setup() {
    densityWidth = 1920;
	densityHeight = 1080;
	// process all but the density on 16th resolution
	simulationWidth = densityWidth / 2;
	simulationHeight = densityHeight / 2;
	windowWidth = ofGetWindowWidth();
	windowHeight = ofGetWindowHeight();

	flowFieldParticlesShaders.setup();
	flowFieldParticlesShaders.setDimensions(densityWidth, densityHeight);
	flowFieldParticlesShaders.load("shaders/flowFieldParticles/bufferA.frag", ofxShadertoy::Buffer::BufferA);
	flowFieldParticlesShaders.load("shaders/flowFieldParticles/bufferB.frag", ofxShadertoy::Buffer::BufferB);
	flowFieldParticlesShaders.load("shaders/flowFieldParticles/bufferC.frag", ofxShadertoy::Buffer::BufferC);
	flowFieldParticlesShaders.load("shaders/flowFieldParticles/bufferD.frag", ofxShadertoy::Buffer::BufferD);
	flowFieldParticlesShaders.load("shaders/flowFieldParticles/image.frag", ofxShadertoy::Buffer::Image);
	flowFieldParticlesShaders.applyTexture(ofxShadertoy::Buffer::BufferA);
	flowFieldParticlesShaders.applyTexture(ofxShadertoy::Buffer::BufferB);
	flowFieldParticlesShaders.applyTexture(ofxShadertoy::Buffer::BufferC);
	flowFieldParticlesShaders.applyTexture(ofxShadertoy::Buffer::BufferD);
	flowFieldParticlesShaders.applyTexture(ofxShadertoy::Buffer::Image);

	flowFieldParticlesShaders.setAdvanceTime(true);
	gui.setup();
	gui.add(toggleGuiDraw.set("debug", false));
	
	gui.add(flowFieldParticlesShaders.parameters);
	if (!ofFile("FlowFieldParticlesScene-settings.xml")) {
		gui.saveToFile("FlowFieldParticlesScene-settings.xml");
	}
	gui.loadFromFile("FlowFieldParticlesScene-settings.xml");
	minimizeGui(&gui);
}

//--------------------------------------------------------------
void FlowFieldParticlesScene::minimizeGui(ofxGuiGroup * _group) {
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
void FlowFieldParticlesScene::updateEnter() {
	// called on first enter update
	if (isEnteringFirst()) {
		ofLogNotice("FlowFieldParticlesScene") << "update enter";
	}

	// fade scene calculates normalized alpha value for us
	ofxFadeScene::updateEnter();

	// finished entering?
	if (!isEntering()) {
		ofLogNotice("FlowFieldParticlesScene") << "update enter done";
	}
}

// normal update
void FlowFieldParticlesScene::update() {
}

// called when scene is exiting, this is just a demo and this
// implementation is not required for this class
void FlowFieldParticlesScene::updateExit() {

	// called on first exit update
	if (isExitingFirst()) {
		ofLogNotice("FlowFieldParticlesScene") << "update exit";
	}

	// fade scene calculates normalized alpha value for us
	ofxFadeScene::updateExit();

	// finished exiting?
	if (!isExiting()) {
		ofLogNotice("FlowFieldParticlesScene") << "update exit done";
	}
}

void FlowFieldParticlesScene::draw() {
	ofPushStyle();
	ofClear(0, 0);
	flowFieldParticlesShaders.begin();
	// Upload uniforms
	ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	flowFieldParticlesShaders.end();
	ofPopStyle();
	
	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(0, 0, 0, 255 * (1.0 - alpha));
	ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	ofDisableAlphaBlending();
	ofPopStyle();
	
	if (toggleGuiDraw) {
		flowFieldParticlesShaders.drawDebug();
		gui.draw();
	}
}

void FlowFieldParticlesScene::exit() {
}

void FlowFieldParticlesScene::onCueConfigEvent(CueEventArgs & args) {
    if (args.cueType != CueType::ConfigUpdate || args.sceneId != 1) return;
    // Apply parameters to GUI controls
    for (const auto& kv : args.parameters) {
        
        // Extend mapping as needed
    }
    ofLogNotice() << "[FlowFieldParticlesScene] Config updated from cue";
}

void FlowFieldParticlesScene::onActorSceneEvent(ActorSceneEventArgs & args) {
// TODO: Handle actor scene event (enter, move, leave)
ofLog() << "FlowToolsScene::onActorSceneEvent " << args.eventType << " actor key: " << args.actorEventArgs.key << " position: " << args.actor->position;
}

void FlowFieldParticlesScene::addActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr) {
	ofAddListener(managerPtr->sceneActorEvent, this, &FlowFieldParticlesScene::onActorSceneEvent);
}

void FlowFieldParticlesScene::removeActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr) {
	ofRemoveListener(managerPtr->sceneActorEvent, this, &FlowFieldParticlesScene::onActorSceneEvent);
}
