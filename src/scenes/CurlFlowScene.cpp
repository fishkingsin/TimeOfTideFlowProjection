//
//  CurlFlowScene.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 21/9/2025.
//

#include "CurlFlowScene.hpp"

#define MAX_NOISE_SCALE 100000

// set the scene name through the base class initializer
CurlFlowScene::CurlFlowScene(std::shared_ptr<ActorManager> _actorManager)
	: ofxFadeScene("CurlFlow")
	, actorManager(_actorManager) {
	setSingleSetup(true); // call setup each time the scene is loaded
	setFade(5000, 5000); // 1 second fade in/out
}

// scene setup
void CurlFlowScene::setup() {
	// Window size
	int w = 1920;
	int h = 1080;

	// GUI setup
	gui.setup();

	//    guiDiscCount.set("Disc Count", 1000, 1, 5000);
	//    gui.add(guiDiscCount);
	gui.add(guiFPS.set("average FPS", 0, 0, 60));
	guiFlow.set("Flow", false);
	gui.add(guiFlow);
	gui.add(toggleGuiDraw.set("show gui (G)", false));
	guiCenter.set("Center", 128, 0, 255);
	gui.add(guiCenter);

	guiWidth.set("Width", 127, 0, 255);
	gui.add(guiWidth);

	guiFrequency1.set("Frequency1", 0.3f, 0.0f, 2.0f);
	gui.add(guiFrequency1);
	guiFrequency2.set("Frequency2", 0.3f, 0.0f, 2.0f);
	gui.add(guiFrequency2);
	guiFrequency3.set("Frequency3", 0.3f, 0.0f, 2.0f);
	gui.add(guiFrequency3);

	guiPhase1.set("Phase1", 0.0f, 0.0f, 10.0f);
	gui.add(guiPhase1);
	guiPhase2.set("Phase2", 2.0f, 0.0f, 10.0f);
	gui.add(guiPhase2);
	guiPhase3.set("Phase3", 4.0f, 0.0f, 10.0f);
	gui.add(guiPhase3);

	guiSpeed.set("Speed", 0.5f, 0.0f, 2.0f);
	gui.add(guiSpeed);

	guiFade.set("Fade", 0.0f, 0.0f, 1.0f);
	gui.add(guiFade);

	noiseScale.set("noiseScale", 500.0f, 1.0f, 1000.0f);
	gui.add(noiseScale);

	guiStep.set("Step", 1000.0f, 10.0f, 3000.0f);
	gui.add(guiStep);
	gui.add(xVol.set("xVol", 0.0f, -10.0f, 10.0f));
	gui.add(yVol.set("yVol", 0.0f, -10.0f, 10.0f));

	guiParticleSize.set("Particle Size", 1.5f, 0.1f, 10.0f);
	gui.add(guiParticleSize);

	guiRainbow.set("Rainbow", true);
	gui.add(guiRainbow);

	guiBaseColor.set("Base Color", ofColor::white, ofColor(0, 0, 0), ofColor(255, 255, 255));
	gui.add(guiBaseColor);

	// Mobile detection omitted; use desktop defaults

	discCount = 10000;

	// Color generation parameters
	center = 128;
	width = 127;
	frequency1 = 0.3;
	frequency2 = 0.3;
	frequency3 = 0.3;
	phase1 = 0;
	phase2 = 2;
	phase3 = 4;

	// Generate color arrays
	red.resize(discCount);
	grn.resize(discCount);
	blu.resize(discCount);
	for (int s = 0; s < discCount; s++) {
		red[s] = round(sin(frequency1 * s + phase1) * width + center);
		grn[s] = round(sin(frequency2 * s + phase2) * width + center);
		blu[s] = round(sin(frequency3 * s + phase3) * width + center);
	}

	// Initialize discs
	discs.clear();
	for (int i = 0; i < discCount; i++) {
		Disc d;
		d.pos.set(ofRandom(w), ofRandom(h));
		d.vel.set(0, 0);
		d.radius = 1;
		d.color = ofColor(red[i], grn[i], blu[i]);
		discs.push_back(d);
	}
	if (!ofFile("CurlFlowScene-settings.xml")) {
		gui.saveToFile("CurlFlowScene-settings.xml");
	}
	gui.loadFromFile("CurlFlowScene-settings.xml");
	// Variables for controls
	discCount = guiDiscCount;
	flow = guiFlow;
	center = guiCenter;
	width = guiWidth;
	frequency1 = guiFrequency1;
	frequency2 = guiFrequency2;
	frequency3 = guiFrequency3;
	phase1 = guiPhase1;
	phase2 = guiPhase2;
	phase3 = guiPhase3;
	speed = guiSpeed;
	fade = guiFade;
	step = guiStep;
	particle_size = guiParticleSize;
	rainbow = guiRainbow;
	baseColor = guiBaseColor;

	ofBackground(0, 0, 0);
	ofSetFrameRate(60);
	ofSetCircleResolution(6);

	rgbaFboFloat.allocate(ofGetWindowWidth(), ofGetWindowHeight(), GL_RGBA32F_ARB); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps

	rgbaFboFloat.begin();
	ofClear(0, 0, 0, 255);
	rgbaFboFloat.end();
	reset();
}

// called when scene is entering, this is just a demo and this
// implementation is not required for this class
void CurlFlowScene::updateEnter() {
	// called on first enter update
	if (isEnteringFirst()) {
		ofLogNotice("CurlFlowScene") << "update enter";
	}

	// fade scene calculates normalized alpha value for us
	ofxFadeScene::updateEnter();

	// finished entering?
	if (!isEntering()) {
		ofLogNotice("CurlFlowScene") << "update enter done";
	}
}

// normal update
void CurlFlowScene::update() {
	guiFPS = (int)(ofGetFrameRate() + 0.5);
	// Sync all variables with GUI
	discCount = guiDiscCount;
	flow = guiFlow;
	center = guiCenter;
	width = guiWidth;
	frequency1 = guiFrequency1;
	frequency2 = guiFrequency2;
	frequency3 = guiFrequency3;
	phase1 = guiPhase1;
	phase2 = guiPhase2;
	phase3 = guiPhase3;
	speed = guiSpeed;
	fade = guiFade;
	step = guiStep;
	particle_size = guiParticleSize;
	rainbow = guiRainbow;
	baseColor = guiBaseColor;

	move();

	// Update velocities using curl noise
	for (int i = 0; i < discs.size(); i++) {
		ofVec2f curl = computeCurl(discs[i].pos.x / step, discs[i].pos.y / step);
		discs[i].vel = curl;
		if (flow) {
			discs[i].vel.x += xVol;
			discs[i].vel.y += yVol;
		}
	}
}

// called when scene is exiting, this is just a demo and this
// implementation is not required for this class
void CurlFlowScene::updateExit() {

	// called on first exit update
	if (isExitingFirst()) {
		ofLogNotice("CurlFlowScene") << "update exit";
	}

	// fade scene calculates normalized alpha value for us
	ofxFadeScene::updateExit();

	// finished exiting?
	if (!isExiting()) {
		ofLogNotice("CurlFlowScene") << "update exit done";
	}
}

// draw
void CurlFlowScene::draw() {
	ofEnableAlphaBlending();
	ofPushStyle();

	rgbaFboFloat.begin();
	ofFill();
	ofSetColor(0, 0, 0, fade * 255);
	ofDrawRectangle(0, 0, rgbaFboFloat.getWidth(), rgbaFboFloat.getHeight());

	// Draw discs
	for (int i = 0; i < discs.size(); i++) {
		if (rainbow) {
			ofSetColor(discs[i].color);
		} else {
			ofSetColor(baseColor);
		}
		ofDrawCircle(discs[i].pos, particle_size);
	}
	rgbaFboFloat.end();

	ofSetColor(255, 255, 255, 255 * alpha);
	rgbaFboFloat.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());

	// Draw GUI
	if (toggleGuiDraw) {
		gui.draw();
	}
	ofPopStyle();
}

// cleanup
void CurlFlowScene::exit() {
}

//--------------------------------------------------------------
void CurlFlowScene::move() {
	int w = ofGetWindowWidth();
	int h = ofGetWindowHeight();
	for (int i = 0; i < discs.size(); i++) {
		Disc & d = discs[i];
		// Boundary logic
		if (d.pos.x < d.radius || d.pos.x > w - d.radius || d.pos.y < d.radius || d.pos.y > h - d.radius) {
			if (flow) {

				if ((abs(xVol) > abs(yVol)) && (d.pos.x < d.radius || d.pos.x > w - d.radius)) {
					d.pos.x = d.radius;
					d.pos.y = ofRandom(h);
				} else if ((abs(xVol) < abs(yVol)) && (d.pos.y < d.radius || d.pos.y > h - d.radius)) {
					d.pos.x = ofRandom(w);
					d.pos.y = d.radius;
				}
			} else {
				d.pos.x = ofRandom(w);
				d.pos.y = ofRandom(h);
			}
		}
		d.pos += d.vel * speed;
	}
}

//--------------------------------------------------------------
ofVec2f CurlFlowScene::computeCurl(float x, float y) {
	float eps = noiseScale / MAX_NOISE_SCALE;
	// dNoise/dy
	float n1 = ofNoise(x, y + eps);
	float n2 = ofNoise(x, y - eps);
	float a = (n1 - n2) / (2 * eps);
	// dNoise/dx
	n1 = ofNoise(x + eps, y);
	n2 = ofNoise(x - eps, y);
	float b = (n1 - n2) / (2 * eps);
	// Curl
	return ofVec2f(a, -b);
}

//--------------------------------------------------------------
void CurlFlowScene::reset() {
	speed = 0.1f;
	step = 1000.0f;
	particle_size = 0.5f;
	rainbow = false;
	fade = 0.05f;

	clearBackground();
}

//--------------------------------------------------------------
void CurlFlowScene::clearBackground() {
	rgbaFboFloat.begin();
	ofFill();
	ofSetColor(0, 0, 0, 255);
	ofDrawRectangle(0, 0, rgbaFboFloat.getWidth(), rgbaFboFloat.getHeight());

	rgbaFboFloat.end();
}

//--------------------------------------------------------------
void CurlFlowScene::randomize() {
// Randomize noise field by reseeding (ofNoise is deterministic, so skip)
clearBackground();
guiSpeed = 0.1f + ofRandom(0.9f);
guiStep = ofRandom(10, 3000);
guiParticleSize = 0.1f + ofRandom(4.9f);
guiRainbow = false;
int c = int(ofRandom(discCount));
guiBaseColor = ofColor(red[c], grn[c], blu[c]);
guiFade = (particle_size >= 2) ? ofRandom(0.1f) : ofRandom(0.01f);
}

void CurlFlowScene::onCueConfigEvent(CueEventArgs & args) {
    if (args.cueType != CueType::ConfigUpdate || args.sceneId != 1) return;
    // Apply parameters to GUI controls
    for (const auto& kv : args.parameters) {
        if (kv.first == "param0") guiSpeed = kv.second;
        if (kv.first == "param1") guiFade = kv.second;
        if (kv.first == "param2") guiStep = kv.second;
        if (kv.first == "param3") guiParticleSize = kv.second;
        // Extend mapping as needed
    }
    ofLogNotice() << "[CurlFlowScene] Config updated from cue";
}

void CurlFlowScene::onActorSceneEvent(ActorSceneEventArgs & args) {
// TODO: Handle actor scene event (enter, move, leave)
ofLog() << "FlowToolsScene::onActorSceneEvent " << args.eventType << " actor key: " << args.actorEventArgs.key << " position: " << args.actor->position;
}

void CurlFlowScene::addActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr) {
	ofAddListener(managerPtr->sceneActorEvent, this, &CurlFlowScene::onActorSceneEvent);
}

void CurlFlowScene::removeActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr) {
	ofRemoveListener(managerPtr->sceneActorEvent, this, &CurlFlowScene::onActorSceneEvent);
}
