//
//  Actor.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 24/8/2025.
//

#include "Actor.hpp"

//--------------------------------------------------------------
void Actor::setup() {
	idleTime = MAX_IDLE_TIME;
	alive = false;
}

//--------------------------------------------------------------
void Actor::setPosition(ofPoint point) {
	lastTime = ofGetElapsedTimef();
	mps = !mps;
	position[mps] = point;
	position[mps].z = 1;
	alive = true;
}

ofVec3f Actor::getPosition() {
	return position[mps];
}

void Actor::update() {
	if (!alive) {
	}
	if (ofGetElapsedTimef() - lastTime > idleTime) {

	} else if (alive == true) {
		alive = false;
		position[mps].z = 0;
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
