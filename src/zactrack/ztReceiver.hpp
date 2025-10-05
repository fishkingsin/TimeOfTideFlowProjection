//
//  ztReceiver.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 24/8/2025.
//

#ifndef ztReceiver_hpp
#define ztReceiver_hpp

#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxGui.h"
#include "ztActorEvent.hpp"
#include "ztActorEventArgs.h"
// listening port
#define PORT 7777

// max 50 tracker in zone
#define MAX_TRACKER 50 // default value

// max number of strings to display
#define NUM_MSG_STRINGS 20

// demonstrates receiving and processing OSC messages with an ofxOscReceiver,
// use in conjunction with the oscSenderExample
class ztReceiver {
public:
	
	void setup();
	void update();
	void draw();
	void minimizeGui(ofxGuiGroup * _group);
	ofxOscReceiver receiver;
	
	ofPoint points[MAX_TRACKER];
	
	int currentMsgString;
	string msgStrings[NUM_MSG_STRINGS];
	float timers[NUM_MSG_STRINGS];
	
	ofParameterGroup		parameters;
	ofParameter<bool>		toggleGuiDraw;
	ofParameter<int>		port;
	ofParameter<bool>		invertX;
	ofParameter<bool>		invertY;
	ofParameter<float>		minX;
	ofParameter<float>		minY;
	ofParameter<float>		maxX;
	ofParameter<float>		maxY;
	ofEvent<ztActorEventArgs> actorEvent;
	ofxPanel gui;
};

#endif /* ztReceiver_hpp */
