//
//  FlowToolsScene.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 21/9/2025.
//

#include "FlowToolsScene.hpp"

#define MAX_NOISE_SCALE 100000

// set the scene name through the base class initializer
FlowToolsScene::FlowToolsScene() : ofxFadeScene("FlowToolsScene"){
	setSingleSetup(true); // call setup each time the scene is loaded
	setFade(5000, 5000); // 1 second fade in/out
}

// scene setup
void FlowToolsScene:: setup() {
	shadertoy.load("shaders/bufferA.frag");
	shadertoy.setAdvanceTime(true);
	densityWidth = 1920;
	densityHeight = 1080;
	// process all but the density on 16th resolution
	simulationWidth = densityWidth / 2;
	simulationHeight = densityHeight / 2;
	windowWidth = ofGetWindowWidth();
	windowHeight = ofGetWindowHeight();
	
	opticalFlow.setup(simulationWidth, simulationHeight);
	velocityBridgeFlow.setup(simulationWidth, simulationHeight);
	densityBridgeFlow.setup(simulationWidth, simulationHeight, densityWidth, densityHeight);
	temperatureBridgeFlow.setup(simulationWidth, simulationHeight);
	fluidFlow.setup(simulationWidth, simulationHeight, densityWidth, densityHeight);
	particleFlow.setup(simulationWidth, simulationHeight, densityWidth, densityHeight);
	densityActorFlow.setup(densityWidth, densityHeight, FT_DENSITY);
	velocityActorFlow.setup(simulationWidth, simulationHeight, FT_VELOCITY);
	
	flows.push_back(&opticalFlow);
	flows.push_back(&velocityBridgeFlow);
	flows.push_back(&densityBridgeFlow);
	flows.push_back(&temperatureBridgeFlow);
	flows.push_back(&fluidFlow);
	flows.push_back(&particleFlow);
	flows.push_back(&densityActorFlow);
	flows.push_back(&velocityActorFlow);
	
	for (auto flow : flows) { flow->setVisualizationFieldSize(glm::vec2(simulationWidth / 2, simulationHeight / 2)); }
	
	actorFlows.push_back(&densityActorFlow);
	actorFlows.push_back(&velocityActorFlow);
	
	flowToolsLogo.load("test.png");
	fluidFlow.addObstacle(flowToolsLogo.getTexture());
	particleFlow.addObstacle(flowToolsLogo.getTexture());
	
	simpleCam.setup(densityWidth, densityHeight, true);
	cameraFbo.allocate(densityWidth, densityHeight);
	ftUtil::zero(cameraFbo);
	
	lastTime = ofGetElapsedTimef();
	
	setupGui();
	
	ofAddListener(receiver.actorEvent, &actorManager, &ActorManager::onActorEvent);
	
	
	
}


//--------------------------------------------------------------
void FlowToolsScene::setupGui() {
	
	gui.setup("settings");
	gui.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	gui.setDefaultFillColor(ofColor(160, 160, 160, 160));
	gui.add(guiFPS.set("average FPS", 0, 0, 60));
	gui.add(guiMinFPS.set("minimum FPS", 0, 0, 60));
//	gui.add(toggleFullScreen.set("fullscreen (F)", false));
	toggleFullScreen.addListener(this, &FlowToolsScene::toggleFullScreenListener);
	gui.add(toggleGuiDraw.set("show gui (G)", true));
	gui.add(useCamera.set("useCamera", true));
	gui.add(useShader.set("useShader", true));
	gui.add(toggleCameraDraw.set("draw camera (C)", true));
	gui.add(toggleActorDraw.set("draw actor (M)", true));
	gui.add(toggleParticleDraw.set("draw particles (P)", true));
	toggleParticleDraw.addListener(this, &FlowToolsScene::toggleParticleDrawListener);
	gui.add(toggleReset.set("reset (R)", false));
	receiver.setup();
	gui.add(receiver.parameters);
	toggleReset.addListener(this, &FlowToolsScene::toggleResetListener);
	
	obsbstacleParameters.setName("obsbstacle");
	obsbstacleParameters.add(obsbstacleFileName.set("image file", "test.png"));
	obsbstacleFileName.addListener(this, &FlowToolsScene::obsbstacleFileNameListener);
	
	visualizationParameters.setName("visualization");
	visualizationParameters.add(visualizationMode.set("mode", FLUID_DEN, INPUT_FOR_DEN, FLUID_DEN));
	visualizationParameters.add(visualizationName.set("name", "fluidFlow Density"));
	visualizationParameters.add(visualizationScale.set("scale", 1, 0.1, 10.0));
	visualizationParameters.add(toggleVisualizationScalar.set("show scalar", false));
	visualizationMode.addListener(this, &FlowToolsScene::visualizationModeListener);
	toggleVisualizationScalar.addListener(this, &FlowToolsScene::toggleVisualizationScalarListener);
	visualizationScale.addListener(this, &FlowToolsScene::visualizationScaleListener);
	
	bool s = true;
	switchGuiColor(s = !s);
	gui.add(obsbstacleParameters);
	gui.add(visualizationParameters);
	for (auto flow : flows) {
		switchGuiColor(s = !s);
		gui.add(flow->getParameters());
	}
	
	if (!ofFile("FlowToolsScene-settings.xml")) { gui.saveToFile("FlowToolsScene-settings.xml"); }
	gui.loadFromFile("FlowToolsScene-settings.xml");
	
//	gui.minimizeAll();
	minimizeGui(&gui);
	
	toggleGuiDraw = true;
}

//--------------------------------------------------------------
void FlowToolsScene::switchGuiColor(bool _switch) {
	ofColor guiHeaderColor[2];
	guiHeaderColor[0].set(160, 160, 80, 200);
	guiHeaderColor[1].set(80, 160, 160, 200);
	ofColor guiFillColor[2];
	guiFillColor[0].set(160, 160, 80, 200);
	guiFillColor[1].set(80, 160, 160, 200);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[_switch]);
	gui.setDefaultFillColor(guiFillColor[_switch]);
}

//--------------------------------------------------------------
void FlowToolsScene::minimizeGui(ofxGuiGroup* _group) {
	for (int i=0; i< _group->getNumControls(); i++) {
		ofxGuiGroup * subGroup  = dynamic_cast<ofxGuiGroup*>(_group->getControl(i));
		if (subGroup) {
			minimizeGui(subGroup);
			_group->minimizeAll();
		}
	}
}

// called when scene is entering, this is just a demo and this
// implementation is not required for this class
void FlowToolsScene:: updateEnter() {
	// called on first enter update
	if(isEnteringFirst()) {
		ofLogNotice("FlowToolsScene") << "update enter";
	}

	// fade scene calculates normalized alpha value for us
	ofxFadeScene::updateEnter();
	
	// finished entering?
	if(!isEntering()) {
		ofLogNotice("FlowToolsScene") << "update enter done";
	}
}

// normal update
void FlowToolsScene:: update() {
	receiver.update();
	actorManager.update();
	float dt = 1.0 / max(ofGetFrameRate(), 1.f); // more smooth as 'real' deltaTime.
	
	simpleCam.update();
	if (simpleCam.isFrameNew()) {
		cameraFbo.begin();
		if (useCamera) {
			simpleCam.draw(cameraFbo.getWidth(), 0, -cameraFbo.getWidth(), cameraFbo.getHeight());  // draw flipped
		} else {
			ofEnableAlphaBlending();
			ofClear(0, 0, 0, 10);
			if (useShader) {
				shadertoy.setDimensions(ofGetWindowWidth(), ofGetWindowHeight());
				shadertoy.begin();
				ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
				shadertoy.end();
			}
			ofDrawCircle(ofGetMouseX(), ofGetMouseY(), ofGetWidth() * densityActorFlow.getRadius());
			ofDisableAlphaBlending();
		}
		cameraFbo.end();
		
		opticalFlow.setInput(cameraFbo.getTexture());
	}
	
	for (auto flow: actorFlows) { flow->update(dt); }

	opticalFlow.update();
	
	velocityBridgeFlow.setVelocity(opticalFlow.getVelocity());
	velocityBridgeFlow.update(dt);
	densityBridgeFlow.setDensity(cameraFbo.getTexture());
	densityBridgeFlow.setVelocity(opticalFlow.getVelocity());
	densityBridgeFlow.update(dt);
	temperatureBridgeFlow.setDensity(cameraFbo.getTexture());
	temperatureBridgeFlow.setVelocity(opticalFlow.getVelocity());
	temperatureBridgeFlow.update(dt);
	
	fluidFlow.addVelocity(velocityBridgeFlow.getVelocity());
	fluidFlow.addDensity(densityBridgeFlow.getDensity());
	fluidFlow.addTemperature(temperatureBridgeFlow.getTemperature());
	for (auto flow: actorFlows) { if (flow->didChange()) { fluidFlow.addFlow(flow->getType(), flow->getTexture()); } }
	fluidFlow.update(dt);
	
	if (toggleParticleDraw) {
		particleFlow.setSpeed(fluidFlow.getSpeed());
		particleFlow.setFlowVelocity(opticalFlow.getVelocity());
		for (auto flow: actorFlows) if (flow->didChange() && flow->getType() == FT_VELOCITY) { particleFlow.addFlowVelocity(flow->getTexture()); }
		particleFlow.setFluidVelocity(fluidFlow.getVelocity());
		particleFlow.setObstacle(fluidFlow.getObstacle());
		particleFlow.update(dt);
	}
}

// called when scene is exiting, this is just a demo and this
// implementation is not required for this class
void FlowToolsScene:: updateExit() {
	
	// called on first exit update
	if(isExitingFirst()) {
		ofLogNotice("FlowToolsScene") << "update exit";
	}
	
	// fade scene calculates normalized alpha value for us
	ofxFadeScene::updateExit();
	
	// finished exiting?
	if(!isExiting()) {
		ofLogNotice("FlowToolsScene") << "update exit done";
	}
}


// draw
void FlowToolsScene:: draw() {
	
	if (toggleCameraDraw.get()) {
//		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		cameraFbo.draw(0, 0, windowWidth, windowHeight);
	}
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	switch(visualizationMode.get()) {
		case INPUT_FOR_DEN:	densityBridgeFlow.drawInput(0, 0, windowWidth, windowHeight); break;
		case INPUT_FOR_VEL: opticalFlow.drawInput(0, 0, windowWidth, windowHeight); break;
		case FLOW_VEL:		opticalFlow.draw(0, 0, windowWidth, windowHeight); break;
		case BRIDGE_VEL:	velocityBridgeFlow.draw(0, 0, windowWidth, windowHeight); break;
		case BRIDGE_DEN:	densityBridgeFlow.draw(0, 0, windowWidth, windowHeight); break;
		case BRIDGE_TMP:	temperatureBridgeFlow.draw(0, 0, windowWidth, windowHeight); break;
		case BRIDGE_PRS:	break;
		case OBSTACLE:		fluidFlow.drawObstacle(0, 0, windowWidth, windowHeight); break;
		case OBST_EDGE:		fluidFlow.drawObstacleOffset(0, 0, windowWidth, windowHeight); break;
		case FLUID_BUOY:	fluidFlow.drawBuoyancy(0, 0, windowWidth, windowHeight); break;
		case FLUID_VORT:	fluidFlow.drawVorticity(0, 0, windowWidth, windowHeight); break;
		case FLUID_DIVE:	fluidFlow.drawDivergence(0, 0, windowWidth, windowHeight); break;
		case FLUID_TMP:		fluidFlow.drawTemperature(0, 0, windowWidth, windowHeight); break;
		case FLUID_PRS:		fluidFlow.drawPressure(0, 0, windowWidth, windowHeight); break;
		case FLUID_VEL:		fluidFlow.drawVelocity(0, 0, windowWidth, windowHeight); break;
		case FLUID_DEN:		fluidFlow.draw(0, 0, windowWidth, windowHeight); break;
		default: break;
	}
//	
	if (toggleParticleDraw) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		particleFlow.draw(0, 0, windowWidth, windowHeight);
	}
	
	if (toggleActorDraw) {
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		densityActorFlow.draw(0, 0, windowWidth, windowHeight);
		velocityActorFlow.draw(0, 0, windowWidth, windowHeight);
	}
	ofEnableAlphaBlending();
	ofSetColor(0, 0, 0, 255 * (1.0 - alpha));
	ofPushStyle();
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
//	ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
	ofPopStyle();
	ofDisableAlphaBlending();
	
//	flowToolsLogo.draw(0, 0, windowWidth, windowHeight);
	
	if (toggleGuiDraw) {
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		drawGui();
	}
	
	
	
}

// cleanup
void FlowToolsScene:: exit() {
	
}



//--------------------------------------------------------------
void FlowToolsScene::drawGui() {
	guiFPS = (int)(ofGetFrameRate() + 0.5);
	
	// calculate minimum fps
	float deltaTime = ofGetElapsedTimef() - lastTime;
	lastTime = ofGetElapsedTimef();
	deltaTimeDeque.push_back(deltaTime);
	
	while (deltaTimeDeque.size() > guiFPS.get())
		deltaTimeDeque.pop_front();
	
	float longestTime = 0;
	for (int i=0; i<deltaTimeDeque.size(); i++){
		if (deltaTimeDeque[i] > longestTime)
			longestTime = deltaTimeDeque[i];
	}
	
	guiMinFPS.set(1.0 / longestTime);
	
	
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	gui.draw();
	receiver.draw();
	ofPopStyle();
}


//--------------------------------------------------------------
void FlowToolsScene::toggleResetListener(bool& _value) {
	if (_value) {
		for (auto flow : flows) { flow->reset(); }
		fluidFlow.addObstacle(flowToolsLogo.getTexture());
	}
	_value = false;
}


void FlowToolsScene::obsbstacleFileNameListener(string& _value) {
	flowToolsLogo.load(_value);
	fluidFlow.reset();
	particleFlow.reset();
	fluidFlow.addObstacle(flowToolsLogo.getTexture());
	particleFlow.addObstacle(flowToolsLogo.getTexture());

}
