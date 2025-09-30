//
//  CueReceiver.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 22/9/2025.
//

#include "CueReceiver.h"


#define ADDRESS_PREFIX "/cue/scene"
//--------------------------------------------------------------
void CueReceiver::setup(){
	// listen on the given port
	receiver.setup(PORT);
	debug = false;
}

//--------------------------------------------------------------
void CueReceiver::update(){
	
	// hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msgStrings[i] = "";
		}
	}
	
	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);
		bool hasMessage = false;
		for (int i = 0 ; i < MAX_TRACKER; i++) {
			int sceneId = i + 1;
			std::string sceneChangeAddr = "/cue/scene" + ofToString(sceneId) + "/change";
			std::string configUpdateAddr = "/cue/scene" + ofToString(sceneId) + "/config";
			
			if (m.getAddress() == sceneChangeAddr) {
				if (debug) {
					ofLog() << "[CueReceiver] Scene Change: " << m.getAddress();
				}
				hasMessage = true;
				CueEventArgs args;
				args.cueType = CueType::SceneChange;
				args.sceneId = sceneId;
				ofNotifyEvent(cueEvent, args);
				break;
			}
			else if (m.getAddress() == configUpdateAddr) {
				if (debug) {
					ofLog() << "[CueReceiver] Config Update: " << m.getAddress();
				}
				hasMessage = true;
				CueEventArgs args;
				args.cueType = CueType::ConfigUpdate;
				args.sceneId = sceneId;
				// Extract parameters from OSC arguments
				for (size_t j = 0; j < m.getNumArgs(); ++j) {
					std::string paramName = "param" + std::to_string(j);
					if (m.getArgType(j) == OFXOSC_TYPE_FLOAT) {
						args.parameters[paramName] = m.getArgAsFloat(j);
					} else if (m.getArgType(j) == OFXOSC_TYPE_INT32) {
						args.parameters[paramName] = static_cast<float>(m.getArgAsInt32(j));
					}
					// Extend as needed for named parameters
				}
				ofNotifyEvent(cueEvent, args);
				break;
			}
		}
		
		if (hasMessage == false){
			
			// unrecognized message: display on the bottom of the screen
			string msgString;
			msgString = m.getAddress();
			msgString += ":";
			for(size_t i = 0; i < m.getNumArgs(); i++){
				
				// get the argument type
				msgString += " ";
				msgString += m.getArgTypeName(i);
				msgString += ":";
				
				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msgString += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msgString += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msgString += m.getArgAsString(i);
				}
				else{
					msgString += "unhandled argument type " + m.getArgTypeName(i);
				}
			}
			
			// add to the list of strings to display
			msgStrings[currentMsgString] = msgString;
			timers[currentMsgString] = ofGetElapsedTimef() + 5.0f;
			currentMsgString = (currentMsgString + 1) % NUM_MSG_STRINGS;
			
			// clear the next line
			msgStrings[currentMsgString] = "";
		}
	}
}


//--------------------------------------------------------------
void CueReceiver::draw(){
	if(debug) {
		ofSetColor(255);
		// draw recent unrecognized messages
		for(int i = 0; i < NUM_MSG_STRINGS; i++){
			ofDrawBitmapStringHighlight(msgStrings[i], 10, 40 + 15 * i);
		}
	}
}
