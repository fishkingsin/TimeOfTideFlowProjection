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
	void update();
	void draw();
	ofPoint position;
	string id;
	float idleTime;
	float lastTime;
	bool alive;
	ofEvent<Actor> actorIdel;
};

