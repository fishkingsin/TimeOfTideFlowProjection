//
//  ActorManager.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 24/8/2025.
//

#pragma once

#include "ofMain.h"
#include "Actor.hpp"
#include "ztActorEventArgs.h"
#include <unordered_map>

class ActorManager {
public:
	
	void setup();
	void update();
	void draw();
	void onActorEvent(ztActorEventArgs &args);
	void onActorIdel(Actor & actor);
	std::unordered_map<string, Actor> actors;
	
};

