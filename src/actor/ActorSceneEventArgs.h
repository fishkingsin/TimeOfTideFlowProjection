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

// ostream operator for ActorSceneEventType
inline std::ostream& operator<<(std::ostream& os, ActorSceneEventType type) {
    switch (type) {
        case ActorSceneEventType::Enter: os << "Enter"; break;
        case ActorSceneEventType::Move: os << "Move"; break;
        case ActorSceneEventType::Leave: os << "Leave"; break;
        default: os << "Unknown"; break;
    }
    return os;
}



struct ActorSceneEventArgs {
    ActorSceneEventType eventType;
    Actor* actor;
    ztActorEventArgs actorEventArgs;
    ActorSceneEventArgs(ActorSceneEventType type, Actor* a, const ztActorEventArgs& args)
        : eventType(type), actor(a), actorEventArgs(args) {}
    ActorSceneEventArgs(ActorSceneEventType type, Actor* a)
        : eventType(type), actor(a), actorEventArgs() {}
};
