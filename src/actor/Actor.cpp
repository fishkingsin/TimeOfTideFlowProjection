//
//  Actor.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 24/8/2025.
//

#include "Actor.hpp"



//--------------------------------------------------------------
void Actor::setup(){
	idleTime = MAX_IDLE_TIME;
	alive = false;
}

//--------------------------------------------------------------
void Actor::setPosition(ofPoint point) {
	lastTime = ofGetElapsedTimef();
	mps = !mps;
	position[mps] = point;
	alive = true;
}
void Actor::update() {
	if (!alive) {
		return;
	}
	if (ofGetElapsedTimef() - lastTime < idleTime) {
		
	} else {
		alive = false;
		ofNotifyEvent(actorIdel, *this);
		//notifiy event
	}
}

//--------------------------------------------------------------
void Actor::draw(float radius) {
	if (!alive) {
		return;
	}
	ofPoint point = position[mps];
	ofDrawCircle(point.x, point.y, radius);

	
}
