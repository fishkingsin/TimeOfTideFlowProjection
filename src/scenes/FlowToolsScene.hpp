//
//  FlowToolsScene.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 21/9/2025.
//

#include "ofMain.h"
#include "ofxAppUtils.h"
#include "ofxGui.h"
#include "ofxFlowTools.h"
#include "ftActorFlow.hpp"
#include "ztReceiver.hpp"
#include "ActorManager.hpp"
#include "ofxShadertoy.h"
#include "CueEventArgs.h"
using namespace flowTools;

enum visualizationTypes{ INPUT_FOR_DEN = 0, INPUT_FOR_VEL, FLOW_VEL, BRIDGE_VEL, BRIDGE_DEN, BRIDGE_TMP, BRIDGE_PRS, OBSTACLE, OBST_EDGE, FLUID_BUOY, FLUID_VORT, FLUID_DIVE, FLUID_TMP, FLUID_PRS, FLUID_VEL, FLUID_DEN };

const vector<string> visualizationNames({"input for density", "input for velocity", "optical flow", "bridge velocity", "bridge density", "bridge temperature", "bridge pressure", "obstacle", "obstacle edge", "fluid buoyancy", "fluid vorticity", "fluid divergence", "fluid temperature", "fluid pressure", "fluid velocity", "fluid density"});

class FlowToolsScene : public ofxFadeScene {
	
public:
	
	void onCueConfigEvent(CueEventArgs & args);
	
	// set the scene name through the base class initializer
	FlowToolsScene():ofxFadeScene("FlowTools") {} ;
	FlowToolsScene(std::shared_ptr<ActorManager> & actorManager_);
	void setup();
	void updateEnter();
	void update();
	void updateExit();
	void draw();
	void exit();
	void onActorSceneEvent(ActorSceneEventArgs & args);
	void addActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr);
	void removeActorSceneEventListener(std::shared_ptr<ActorManager> & managerPtr);
	
	int		densityWidth, densityHeight, simulationWidth, simulationHeight, windowWidth, windowHeight;
	
	
	vector< ftFlow* >		flows;
	ftOpticalFlow			opticalFlow;
	ftVelocityBridgeFlow	velocityBridgeFlow;
	ftDensityBridgeFlow		densityBridgeFlow;
	ftTemperatureBridgeFlow temperatureBridgeFlow;
	ftFluidFlow				fluidFlow;
	vector< ftActorFlow* >	actorFlows;
	ftActorFlow				densityActorFlow;
	ftActorFlow				velocityActorFlow;
	ftParticleFlow			particleFlow;
	
	ofImage					flowToolsLogo;
	ofParameterGroup		obsbstacleParameters;
	ofParameter<string>		obsbstacleFileName;
	ofParameterGroup		visualizationParameters;
	ofParameter<int>		visualizationMode;
	ofParameter<string>		visualizationName;
	ofxShadertoy shadertoy;
	ofParameter<float>		visualizationScale;
	ofParameter<bool>		toggleVisualizationScalar;
	void obsbstacleFileNameListener(string& _value);
	void visualizationModeListener(int& _value) 			{ visualizationName.set(visualizationNames[_value]); }
	void visualizationScaleListener(float& _value)			{ for (auto flow : flows) { flow->setVisualizationScale(_value); } }
	void toggleVisualizationScalarListener(bool &_value)	{ for (auto flow : flows) { flow->setVisualizationToggleScalar(_value); } }
	
	ofVideoGrabber		simpleCam;
	ofFbo				cameraFbo;
	
	ofxPanel			gui;
	void				setupGui();
	void				minimizeGui(ofxGuiGroup* _group);
	void				switchGuiColor(bool _switch);
	ofParameter<float>	guiFPS;
	ofParameter<float>	guiMinFPS;
	ofParameter<bool>	toggleFullScreen;
	ofParameter<bool>	toggleGuiDraw;
	ofParameter<bool>	useCamera;
	ofParameter<bool>	useShader;
	
	ofParameter<bool>	toggleCameraDraw;
	ofParameter<bool>	toggleParticleDraw;
	ofParameter<bool>	toggleActorDraw;
	ofParameter<bool>	toggleReset;
	
	void				toggleFullScreenListener(bool& _value) { ofSetFullscreen(_value);}
	void				toggleResetListener(bool& _value);
	void				toggleParticleDrawListener(bool& _value) { if (_value) { particleFlow.reset(); } }
	void 				windowResized(ofResizeEventArgs & _resize){ windowWidth = _resize.width; windowHeight = _resize.height; }
	
	void				drawGui();
	deque<float>		deltaTimeDeque;
	float				lastTime;
	shared_ptr<ActorManager> actorManager;
};
