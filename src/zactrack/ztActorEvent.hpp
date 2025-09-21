//
//  ztActorEvent.hpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 24/8/2025.
//

#pragma once

#include "ofMain.h"
#include "Actor.hpp"
#include "ztActorEventArgs.h"



class ztActorEvent: ofEvent<ztActorEventArgs> {
public:
	ztActorEvent& setArgs(ztActorEventArgs args);
	ztActorEventArgs args;
	
	
};
