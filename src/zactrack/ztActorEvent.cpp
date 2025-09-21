//
//  ztActorEvent.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 24/8/2025.
//

#include "ztActorEvent.hpp"

ztActorEvent& ztActorEvent::setArgs(ztActorEventArgs args) {
	this->args = args;
	return *this;
}
