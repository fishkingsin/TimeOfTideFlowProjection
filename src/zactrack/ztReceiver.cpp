//
//  ztReceiver.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 24/8/2025.
//

#include "ztReceiver.hpp"

#define ADDRESS_PREFIX "/zactrack"
//--------------------------------------------------------------
void ztReceiver::setup(){
	// listen on the given port
	ofLog() << "listening for osc messages on port " << PORT;
	receiverPanel.setup("zactrack_receiver");
	parameters.setName("zactrack receiver");
	parameters.add(toggleGuiDraw.set("toggleGuiDraw", false));
	parameters.add(port.set("port", PORT));
	parameters.add(invertX.set("Invert X", false));
	parameters.add(invertY.set("Invert Y", false));
	parameters.add(minX.set("min X", -6000, -10000, 10000));
	parameters.add(minY.set("min Y", -6000, -10000, 10000));
	parameters.add(maxX.set("max X", -6000, -10000, 10000));
	parameters.add(maxY.set("max Y", -6000, -10000, 10000));
	
	receiver.setup(PORT);
	currentMsgString = (currentMsgString + 1) % NUM_MSG_STRINGS;
	
	
	receiverPanel.add(parameters);
	if (!ofFile("zactrack_receiver-settings.xml")) { receiverPanel.saveToFile("zactrack_receiver-settings.xml"); }
	receiverPanel.loadFromFile("zactrack_receiver-settings.xml");
}

//--------------------------------------------------------------
void ztReceiver::update(){
	
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
				int rawX = m.getArgAsInt(0) * (invertX ? -1 : 1);
				int rawY = m.getArgAsInt(1) * (invertY ? -1 : 1);
				points[i].set(
							  ofMap(rawX, minX, maxX, 0, ofGetWidth())
							  ,
							  ofMap(rawY, minY, maxY, 0, ofGetHeight()));

				if (toggleGuiDraw) {
					ofLog() << "points " << i << " " << points[i].x << " " << points[i].y;
					ofLog() << m.getAddress() << " args 1 " << m.getArgAsInt(0)
					<< " args 2 " << m.getArgAsInt(1)
					<< " args 3 " << m.getArgAsInt(2);
				}
				ztActorEventArgs args;
				args.key = key;
				args.point = points[i];
				ofNotifyEvent(actorEvent, args);
				hasMessage = true;
			}
		}
		
		if (hasMessage == false && toggleGuiDraw){
			
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
void ztReceiver::draw(){
	if(toggleGuiDraw) {
		ofPushStyle();
		ofSetColor(255);
		// draw recent unrecognized messages
		for(int i = 0; i < NUM_MSG_STRINGS; i++){
			ofDrawBitmapStringHighlight(msgStrings[i], 10, 40 + 15 * i);
		}
		
		
		
		
		for(int i = 0; i < 30; i++){
			string buf = "id :" + ofToString(i);
			ofDrawBitmapStringHighlight(buf, points[i].x, points[i].y);
			
			ofDrawCircle(points[i].x, points[i].y, 10);
		}
		ofPopStyle();
	}
	if (toggleGuiDraw) {
		receiverPanel.draw();
	}
}
