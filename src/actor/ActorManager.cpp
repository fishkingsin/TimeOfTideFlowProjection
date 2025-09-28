//
//  ActorManager.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 24/8/2025.
//

#include "ActorManager.hpp"

#include "ActorSceneEventArgs.h"

void ActorManager::notifySceneActorEvent(ActorSceneEventType eventType, Actor& actor, const ztActorEventArgs* args) {
	if (args) {
		ActorSceneEventArgs sceneArgs(eventType, &actor, *args);
		ofNotifyEvent(sceneActorEvent, sceneArgs, this);
	} else {
		ActorSceneEventArgs sceneArgs(eventType, &actor);
		ofNotifyEvent(sceneActorEvent, sceneArgs, this);
	}
}

//--------------------------------------------------------------
void ActorManager::setup(){
	
}

//--------------------------------------------------------------
void ActorManager::update() {
	for (auto actor = actors.begin(); actor != actors.end(); ++actor) {
		actor->second.update();
	}
}


//--------------------------------------------------------------
void ActorManager::draw() {
	ofSetColor(ofColor::red);
	for (auto actor = actors.begin(); actor != actors.end(); ++actor) {
		actor->second.draw();
	}
}

void ActorManager::onActorEvent(ztActorEventArgs &args) {
	std::unordered_map<std::string, Actor>::const_iterator got = actors.find(args.key);
	if (got == actors.end()) {
		ofLogError() << "actor with key " << args.key << " no found create new";
		Actor actor;
		actor.id = args.key;
		actor.setPosition(args.point);
		ofAddListener(actor.actorIdel, this, &ActorManager::onActorIdel);
		actors.insert(std::make_pair(args.key, actor));
		notifySceneActorEvent(ActorSceneEventType::Enter, actor, &args);
	} else {
		ofLog() << "actor with key " << args.key << " updated to position " << args.point;
		actors[args.key].setPosition(args.point);
		notifySceneActorEvent(ActorSceneEventType::Move, actors[args.key], &args);
	}
}

void ActorManager::onActorIdel(Actor & actor) {
	ofLog() << "actor idel with key " << actor.id << " erase and remove listener";
	notifySceneActorEvent(ActorSceneEventType::Leave, actor);
	actors.erase(actor.id);
	ofRemoveListener(actor.actorIdel, this, &ActorManager::onActorIdel);
}
