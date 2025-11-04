//
//  Actor.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 24/8/2025.
//

#pragma once

#include "ofMain.h"
#define MAX_IDLE_TIME 10 // seconds
class Actor {
public:
	
	void setup();
	void setPosition(ofPoint point);
	ofVec3f getPosition();
	void update();
	void draw(float radius);
	ofVec3f position[2];
	bool mps;
	string id;
	unsigned int index;
	float idleTime = MAX_IDLE_TIME;
	float lastTime;
	bool alive;
	ofEvent<Actor> actorIdel;
	
};

