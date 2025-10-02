//
//  CueReceiver.h
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 22/9/2025.
//

//
//  ztReceiver.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 24/8/2025.
//

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxGui.h"
#include "CueEventArgs.h"
// listening port
#define PORT_2 7778

// max 50 tracker in zone
#define MAX_TRACKER 50 // default value

// max number of strings to display
#define NUM_MSG_STRINGS 20

// demonstrates receiving and processing OSC messages with an ofxOscReceiver,
// use in conjunction with the oscSenderExample
class CueReceiver {
public:
	
	void setup();
	void update();
	void draw();
	ofxOscReceiver receiver;
	int currentMsgString;
	string msgStrings[NUM_MSG_STRINGS];
	float timers[NUM_MSG_STRINGS];
	bool debug;
	
	ofEvent<CueEventArgs> cueEvent;
};
