//
//  ActorManager.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 24/8/2025.
//

#include "ActorManager.hpp"

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
	} else {
		ofLog() << "actor with key " << args.key << " updated to position " << args.point;
		actors[args.key].setPosition(args.point);
	}
}

void ActorManager::onActorIdel(Actor & actor) {
	ofLog() << "actor idel with key " << actor.id << " erase and remove listener";
	actors.erase(actor.id);
	ofRemoveListener(actor.actorIdel, this, &ActorManager::onActorIdel);
}
 
