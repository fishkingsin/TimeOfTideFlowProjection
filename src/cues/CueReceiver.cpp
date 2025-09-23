//
//  CueReceiver.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 22/9/2025.
//

#include "CueReceiver.hpp"


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
			int index = i + 1; // offset from zt start from 1
			string key = ADDRESS_PREFIX + ofToString(i + 1);
			if (m.getAddress().starts_with(key)) {
				
				if (debug) {
					
					ofLog() << m.getAddress() << " args 1 " << m.getArgAsInt(0)
					<< " args 2 " << m.getArgAsInt(1)
					<< " args 3 " << m.getArgAsInt(2);
				}
				
				hasMessage = true;
				CueEventArgs args;
				ofNotifyEvent(cueEvent, args);
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
