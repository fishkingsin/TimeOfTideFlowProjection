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
#include "ActorSceneEventArgs.h"


class ActorManager {
public:
	
	void setup();
	void update();
	void draw(float radius);
	void onActorEvent(ztActorEventArgs &args);
	void onActorIdel(Actor & actor);
	void notifySceneActorEvent(ActorSceneEventType eventType, shared_ptr<Actor> actor, const ztActorEventArgs* args = nullptr);
	ofEvent<ActorSceneEventArgs> sceneActorEvent;
	std::unordered_map<string, std::shared_ptr<Actor>> actors;
	ofParameter<bool> debug;
};
