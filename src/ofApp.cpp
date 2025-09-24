#include "ofApp.h"
#include "scenes/scenes.h"
//--------------------------------------------------------------
void ofApp::setup(){
	
	// setup for nice jaggy-less rendering
	ofSetVerticalSync(true);
	ofBackground(0, 0, 0);

	// setup the render size (working area)
	transformer.setRenderSize(ofGetWidth(), ofGetHeight());

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
	sceneManager.add(new SinglePassFlowFieldScene());
	flowToolsScene = (FlowToolsScene*) sceneManager.add(new FlowToolsScene());
	curlFlowScene = (CurlFlowScene*) sceneManager.add(new CurlFlowScene()); // save pointer
//	sceneManager.add(new LineScene());
	sceneManager.setup(true); // true = setup all the scenes now (not on the fly)
	ofSetLogLevel("ofxSceneManager", OF_LOG_VERBOSE); // lets see whats going on inside
	
	// start with a specific scene
	// set now to true in order to ignore the scene fade and change now
	sceneManager.gotoScene("CurlFlow", true);
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
}


//--------------------------------------------------------------
void ofApp::update(){
	
	
#ifdef HAVE_OFX_GUI
	// update the transform panel when in debug mode
	if(isDebug()) {
		panel.update();
	}
#endif
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	
	
	// the current scene is automatically drawn before this function

	// show the render area edges with a white rect
	if(isDebug()) {
		ofNoFill();
		ofSetColor(255);
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofDrawRectangle(1, 1, getRenderWidth()-2, getRenderHeight()-2);
		ofFill();
	}
	
	// drop out of the auto transform space back to OF screen space
	transformer.pop();
	
	#ifdef HAVE_OFX_GUI
		// draw the transform panel when in debug mode
		if(isDebug()) {
			panel.draw();
		}
	#endif
	
	// draw current scene info using the ofxBitmapString stream interface
	// to ofDrawBitmapString
	ofSetColor(200);
	ofxBitmapString(12, ofGetHeight()-8)
		<< "Current Scene: " << sceneManager.getCurrentSceneIndex()
		<< " " << sceneManager.getCurrentSceneName() << endl;
	
	// go back to the auto transform space
	//
	// this is actually done automatically if the transformer is set but
	// included here for completeness
	transformer.push();

	// the warp editor is drawn automatically after this function

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
		case 'd':
			bDebug = !bDebug;
			break;
		default: break;
		case '1': flowToolsScene->visualizationMode.set(INPUT_FOR_DEN); break;
		case '2': flowToolsScene->visualizationMode.set(INPUT_FOR_VEL); break;
		case '3': flowToolsScene->visualizationMode.set(FLOW_VEL); break;
		case '4': flowToolsScene->visualizationMode.set(BRIDGE_VEL); break;
		case '5': flowToolsScene->visualizationMode.set(BRIDGE_DEN); break;
		case '6': flowToolsScene->visualizationMode.set(FLUID_VORT); break;
		case '7': flowToolsScene->visualizationMode.set(FLUID_TMP); break;
		case '8': flowToolsScene->visualizationMode.set(FLUID_PRS); break;
		case '9': flowToolsScene->visualizationMode.set(FLUID_VEL); break;
		case '0': flowToolsScene->visualizationMode.set(FLUID_DEN); break;
		case 'G': flowToolsScene->toggleGuiDraw = !flowToolsScene->toggleGuiDraw; break;
		case 'F': flowToolsScene->toggleFullScreen.set(!flowToolsScene->toggleFullScreen.get()); break;
		case 'C': flowToolsScene->toggleCameraDraw.set(!flowToolsScene->toggleCameraDraw.get()); break;
		case 'M': flowToolsScene->toggleActorDraw.set(!flowToolsScene->toggleActorDraw.get()); break;
		case 'R': flowToolsScene->toggleReset.set(!flowToolsScene->toggleReset.get()); break;
		case 'P': flowToolsScene->toggleParticleDraw.set(!flowToolsScene->toggleParticleDraw.get()); break;
		case OF_KEY_LEFT:
			sceneManager.prevScene();
			break;
			
		case OF_KEY_RIGHT:
			sceneManager.nextScene();
			break;
			
		case OF_KEY_DOWN:
			if(sceneManager.getCurrentScene()) { // returns NULL if no scene selected
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
			break;
	}
}
