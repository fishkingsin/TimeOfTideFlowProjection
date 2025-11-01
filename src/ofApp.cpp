#include "ofApp.h"
#include "scenes/scenes.h"

void ofApp::setup() {
	sceneChangeTime = 30000; // 30 seconds
	lastSceneChangeTime = 0;
	cueReceiver.setup();
	actorReceiver.setup();
	ofAddListener(cueReceiver.cueEvent, this, &ofApp::onCueEvent);

	// setup for nice jaggy-less rendering
	ofSetVerticalSync(true);
	ofBackground(0, 0, 0);
	ActorManager actorManager;
	actorManager.setup();
	actorManagerPtr = std::make_shared<ActorManager>(actorManager);
	ofAddListener(actorReceiver.actorEvent, actorManagerPtr.get(), &ActorManager::onActorEvent);

	// setup the render size (working area)

	// turn on transform origin translation and scaling to screen size,
	// disable quad warping, and enable aspect ratio and centering when scaling
	transformer.setTransforms(true, true, false, true, true);

	// set the ofxApp transformer so it's automatically applied in draw()
	setTransformer(&transformer);

#ifdef HAVE_OFX_GUI
	// setup transform panel with transformer pointer,
	// loads settings & quad warper xml files if found
	panel.setup(&transformer);
#endif

	// load scenes

	//	sceneManager.add(new GPUCurlFlowScene());

	curlFlowScene = static_cast<CurlFlowScene *>(sceneManager.add(new CurlFlowScene(actorManagerPtr))); // save pointer
	flowToolsScene = static_cast<FlowToolsScene *>(sceneManager.add(new FlowToolsScene(actorManagerPtr)));
//	flowFieldParticlesScene = static_cast<FlowFieldParticlesScene *>(sceneManager.add(new FlowFieldParticlesScene(actorManagerPtr)));
	singlePassFlowFieldScene = static_cast<SinglePassFlowFieldScene *>(sceneManager.add(new SinglePassFlowFieldScene(actorManagerPtr)));
	ofAddListener(cueReceiver.cueEvent, curlFlowScene, &CurlFlowScene::onCueConfigEvent);
	ofAddListener(cueReceiver.cueEvent, flowToolsScene, &FlowToolsScene::onCueConfigEvent);
	ofAddListener(cueReceiver.cueEvent, singlePassFlowFieldScene, &SinglePassFlowFieldScene::onCueConfigEvent);
	//	sceneManager.add(new LineScene());
	sceneManager.setup(true); // true = setup all the scenes now (not on the fly)
	ofSetLogLevel("ofxSceneManager", OF_LOG_VERBOSE); // lets see whats going on inside

	// start with a specific scene
	// set now to true in order to ignore the scene fade and change now
	sceneManager.gotoScene(singlePassFlowFieldScene->getName(), true);
	singlePassFlowFieldScene->addActorSceneEventListener(actorManagerPtr);
	lastScene = sceneManager.getCurrentSceneIndex();

	// overlap scenes when transitioning
	sceneManager.setOverlap(false);

	// attach scene manager to this ofxApp so it's called automatically,
	// you can also call the callbacks (update, draw, keyPressed, etc) manually
	// if you don't set it
	//
	// you can also turn off the auto sceneManager update and draw calls with:
	// setSceneManagerUpdate(false);
	// setSceneManagerDraw(false);
	//
	// the input callbacks in your scenes will be called if they are implemented
	//
	setSceneManager(&sceneManager);
	ofHideCursor();
}

//--------------------------------------------------------------
void ofApp::update() {

#ifdef HAVE_OFX_GUI
	// update the transform panel when in debug mode
	if (isDebug()) {
		panel.update();
	}
#endif
	actorReceiver.update();
	actorManagerPtr->update();
	cueReceiver.update();

	// every 3 min change to next scene
	if (!isShowingGui()) {
		if (ofGetElapsedTimeMillis() - lastSceneChangeTime > sceneChangeTime) {
			sceneManager.nextScene();
			lastSceneChangeTime = ofGetElapsedTimeMillis();
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	// the current scene is automatically drawn before this function

	// show the render area edges with a white rect
	if (isDebug()) {
		ofNoFill();
		ofSetColor(255);
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofDrawRectangle(1, 1, getRenderWidth() - 2, getRenderHeight() - 2);
		ofFill();
	}

	// drop out of the auto transform space back to OF screen space
	transformer.pop();

#ifdef HAVE_OFX_GUI
	// draw the transform panel when in debug mode
	if (isDebug()) {
		panel.draw();
	}
#endif
	if (isDebug()) {
		// draw current scene info using the ofxBitmapString stream interface
		// to ofDrawBitmapString
		ofSetColor(200);
		ofxBitmapString(12, ofGetHeight() - 8)
			<< "Current Scene: " << sceneManager.getCurrentSceneIndex()
			<< " " << sceneManager.getCurrentSceneName() << endl;
	}
	// go back to the auto transform space
	//
	// this is actually done automatically if the transformer is set but
	// included here for completeness
	transformer.push();

#ifdef HAVE_OFX_GUI
	actorReceiver.draw();

//	actorManager.draw(10);
#endif
	// the warp editor is drawn automatically after this function
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case 'd':
		bDebug = !bDebug;
		break;
	default:
		break;
	case '1':
		flowToolsScene->visualizationMode.set(INPUT_FOR_DEN);
		break;
	case '2':
		flowToolsScene->visualizationMode.set(INPUT_FOR_VEL);
		break;
	case '3':
		flowToolsScene->visualizationMode.set(FLOW_VEL);
		break;
	case '4':
		flowToolsScene->visualizationMode.set(BRIDGE_VEL);
		break;
	case '5':
		flowToolsScene->visualizationMode.set(BRIDGE_DEN);
		break;
	case '6':
		flowToolsScene->visualizationMode.set(FLUID_VORT);
		break;
	case '7':
		flowToolsScene->visualizationMode.set(FLUID_TMP);
		break;
	case '8':
		flowToolsScene->visualizationMode.set(FLUID_PRS);
		break;
	case '9':
		flowToolsScene->visualizationMode.set(FLUID_VEL);
		break;
	case '0':
		flowToolsScene->visualizationMode.set(FLUID_DEN);
		break;
	case 'G':
		flowToolsScene->toggleGuiDraw = !flowToolsScene->toggleGuiDraw;
		break;
	case 'F':
		ofToggleFullscreen();
		break;
	case 'C':
		flowToolsScene->toggleCameraDraw.set(!flowToolsScene->toggleCameraDraw.get());
		break;
	case 'M':
		flowToolsScene->toggleActorDraw.set(!flowToolsScene->toggleActorDraw.get());
		break;
	case 'R':
		flowToolsScene->toggleReset.set(!flowToolsScene->toggleReset.get());
		break;
	case 'P':
		flowToolsScene->toggleParticleDraw.set(!flowToolsScene->toggleParticleDraw.get());
		break;
	case OF_KEY_LEFT:
		sceneManager.prevScene();
		break;

	case OF_KEY_RIGHT:
		sceneManager.nextScene();
		break;

	case OF_KEY_DOWN:
		if (sceneManager.getCurrentScene()) { // returns NULL if no scene selected
			lastScene = sceneManager.getCurrentSceneIndex();
		}
		sceneManager.noScene();
		break;

	case OF_KEY_UP:
		sceneManager.gotoScene(lastScene);
		break;

	case 'g':
		if (curlFlowScene != NULL) curlFlowScene->toggleGuiDraw = !curlFlowScene->toggleGuiDraw;
		if (flowToolsScene != NULL) flowToolsScene->toggleGuiDraw = !flowToolsScene->toggleGuiDraw;
		if (singlePassFlowFieldScene != NULL) singlePassFlowFieldScene->toggleGuiDraw = !singlePassFlowFieldScene->toggleGuiDraw;
//		if (flowFieldParticlesScene != NULL) flowFieldParticlesScene->toggleGuiDraw = !flowFieldParticlesScene->toggleGuiDraw;
		actorReceiver.toggleGuiDraw = !actorReceiver.toggleGuiDraw;
			if (isShowingGui()) {
				ofShowCursor();
			} else {
				ofHideCursor();
			}
		break;
	}
}

//--------------------------------------------------------------
void ofApp::onCueEvent(CueEventArgs & args) {
	if (args.cueType == CueType::SceneChange) {
		// Map sceneId to scene name
		std::string sceneName;
		switch (args.sceneId) {
		case 1:
			sceneName = "CurlFlow";
			break;
		case 2:
			sceneName = "FlowTools";
			break;
		case 3:
			sceneName = "SinglePassFlowField";
			break;
		default:
			return; // Unknown scene
		}
		sceneManager.gotoScene(sceneName, true);
		lastScene = sceneManager.getCurrentSceneIndex();
		ofLogNotice() << "[ofApp] Scene changed to " << sceneName;
	}
	// Optionally handle config update cues here
}

bool ofApp::isShowingGui() {
	// store all point as instance
	bool isGuiVisible = false;
	if (curlFlowScene != NULL) isGuiVisible = curlFlowScene->toggleGuiDraw || isGuiVisible;
	if (flowToolsScene != NULL) isGuiVisible = flowToolsScene->toggleGuiDraw || isGuiVisible;
	if (singlePassFlowFieldScene != NULL) isGuiVisible = singlePassFlowFieldScene->toggleGuiDraw || isGuiVisible;
//	if (flowFieldParticlesScene != NULL) isGuiVisible = flowFieldParticlesScene->toggleGuiDraw || isGuiVisible;
//	isGuiVisible = actorReceiver.toggleGuiDraw || isGuiVisible;
	return isGuiVisible;
}
