// ActorSceneEventArgs.h
// TimeOfTideFlowProjection

#pragma once

#include "Actor.hpp"
#include "ActorManager.hpp"
#include "ztActorEventArgs.h"

enum class ActorSceneEventType {
	Enter,
	Move,
	Leave
};


struct ActorSceneEventArgs {
    ActorSceneEventType eventType;
    Actor* actor;
    ztActorEventArgs actorEventArgs;
    ActorSceneEventArgs(ActorSceneEventType type, Actor* a, const ztActorEventArgs& args)
        : eventType(type), actor(a), actorEventArgs(args) {}
    ActorSceneEventArgs(ActorSceneEventType type, Actor* a)
        : eventType(type), actor(a), actorEventArgs() {}
};
