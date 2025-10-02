//
//  ActorManager.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 24/8/2025.
//

#include "ActorManager.hpp"

#include "ActorSceneEventArgs.h"

void ActorManager::notifySceneActorEvent(ActorSceneEventType eventType, shared_ptr<Actor> actor, const ztActorEventArgs* args) {
	if (args) {
		ActorSceneEventArgs sceneArgs(eventType, actor, *args);
		ofNotifyEvent(sceneActorEvent, sceneArgs, this);
	} else {
		ActorSceneEventArgs sceneArgs(eventType, actor);
		ofNotifyEvent(sceneActorEvent, sceneArgs, this);
	}
}

//--------------------------------------------------------------
void ActorManager::setup(){
	debug = false;
}

//--------------------------------------------------------------
void ActorManager::update() {
}


//--------------------------------------------------------------
void ActorManager::draw(float radius) {
	ofPushStyle();
	ofSetColor(ofColor::white);
	for (auto actor = actors.begin(); actor != actors.end(); ++actor) {
		actor->second->draw(radius);
	}
	ofPopStyle();
}

void ActorManager::onActorEvent(ztActorEventArgs &args) {
	std::unordered_map<std::string, std::shared_ptr<Actor>>::const_iterator got = actors.find(args.key);
	if (got == actors.end()) {
		if (debug) {
			ofLogError() << "actor with key " << args.key << " no found create new";
		}
		Actor actor;
		actor.id = args.key;
		actor.setPosition(args.point);
		ofAddListener(actor.actorIdel, this, &ActorManager::onActorIdel);
		auto actor_ptr = std::make_shared<Actor>(actor);
		actors.insert(std::make_pair(args.key, actor_ptr));
		notifySceneActorEvent(ActorSceneEventType::Enter, actor_ptr, &args);
	} else {
		if (debug) {
			ofLog() << "actor with key " << args.key << " updated to position " << args.point;
		}
		actors[args.key]->setPosition(args.point);
		notifySceneActorEvent(ActorSceneEventType::Move, actors[args.key], &args);
	}
}

void ActorManager::onActorIdel(Actor & actor) {
	ofLog() << "actor idel with key " << actor.id << " erase and remove listener";
	ofRemoveListener(actor.actorIdel, this, &ActorManager::onActorIdel);
	notifySceneActorEvent(ActorSceneEventType::Leave, std::make_shared<Actor>(actor));
	actors.erase(actor.id);
	
}
