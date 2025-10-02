//
//  ftActorFlow.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 23/8/2025.
//

#include "ftActorFlow.hpp"

namespace flowTools {
	
	ftActorFlow::ftActorFlow() {
		ofAddListener(ofEvents().mouseMoved, this, &ftActorFlow::mouseMoved);
		ofAddListener(ofEvents().mouseDragged, this, &ftActorFlow::mouseDragged);
	}
	
	ftActorFlow::~ftActorFlow() {
		ofRemoveListener(ofEvents().mouseMoved, this, &ftActorFlow::mouseMoved);
		ofRemoveListener(ofEvents().mouseDragged, this, &ftActorFlow::mouseDragged);
	}
	
	//--------------------------------------------------------------
	void ftActorFlow::setup(int _width, int _height, ftFlowForceType _type) {
		if (_type < FT_DENSITY || _type > FT_OBSTACLE) {
			ofLogWarning("ftActorFlow") << "Type " << ftFlowForceNames[_type] << " not supported";
			return;
		}
		type = _type;
		ftFlow::allocate(_width, _height, ftUtil::getInternalFormatFromType(type));
		
		mousePositionsSet = false;
		force = glm::vec4(0,0,0,0);
		
		parameters.add(pSpeed.set("speed", 50, 0, 100));
		parameters.add(pPersistent.set("persistent", false));
		pPersistent.addListener(this, &ftActorFlow::pPersistentListener);
		parameters.add(pRadius.set("radius", 0.035, 0, .1));
		parameters.add(pSmooth.set("smooth", 1.0, 0, 1));
		parameters.add(pInverse.set("inverse", false));
		switch (type) {
			case FT_DENSITY:
				parameters.setName("mouse density");
				parameters.add(pColor.set("color", ofFloatColor(.5,.5,.5,.5), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
				break;
			case FT_VELOCITY:
				parameters.setName("mouse velocity");
				break;
			case FT_PRESSURE:
				parameters.setName("mouse pressure");
				break;
			case FT_TEMPERATURE:
				parameters.setName("mouse temperature");
				break;
			case FT_OBSTACLE:
				parameters.setName("mouse obstacle");
				pSmooth.set(0);
				parameters.remove("inverse");
				parameters.remove("smooth");
				parameters.remove("speed");
				break;
			default:
				break;
		}

		bMouseDragged = false;
		bFlowChanged = false;
	}
	
	//--------------------------------------------------------------
	void ftActorFlow::update(float _deltaTime) {
		bFlowChanged = false;
		if (!pPersistent) { ftFlow::reset(); }
		if (bMouseDragged) {
			float inv = pInverse? -1: 1;
			glm::vec2 vel = (mousePositions[mps] - mousePositions[!mps]) * inv;
			float mag = glm::length(vel) * inv;
			
			switch (type) {
				case FT_DENSITY:
					force = glm::vec4(pColor->r, pColor->g, pColor->b, pColor->a) * mag;
					break;
				case FT_VELOCITY:
					force = glm::vec4(vel, 0, 0);
					break;
				case FT_PRESSURE:
				case FT_TEMPERATURE:
					force = glm::vec4(mag, 0, 0, 0);
					break;
				case FT_OBSTACLE:
					force = pInverse? glm::vec4(0, 0, 0, 0): glm::vec4(1, 0, 0, 0);
					break;
				default:
					break;
			}
			
			glm::vec2 startPosition = mousePositions[!mps] * glm::vec2(inputWidth, inputHeight);
			glm::vec2 endPosition = mousePositions[mps] * glm::vec2(inputWidth, inputHeight);
			float radius = pRadius.get() * inputWidth;
			
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			inputFbo.swap();
			actorShader.update(inputFbo.get(),
							   force,
							   endPosition,
							   radius,
							   pSmooth.get());
			
			if (pPersistent) {
				addInput(inputFbo.getBackTexture());
			} else {
				resetOutput();
				add(outputFbo, inputFbo.getTexture(), pSpeed.get() * _deltaTime * ofGetFrameRate());
			}
			
			ofPopStyle();
			
			bMouseDragged = false;
			bFlowChanged = true;
		}
		if (pPersistent) {
			resetOutput();
			add(outputFbo, inputFbo.getTexture(), pSpeed.get() * _deltaTime);
			bFlowChanged = true;
		}
	}
	
	//--------------------------------------------------------------
	void ftActorFlow::mouseDragged( ofMouseEventArgs& _mouse ) {
		mouseMoved(_mouse);
		bMouseDragged = true;
	}
	
	//--------------------------------------------------------------
	void ftActorFlow::mouseMoved( ofMouseEventArgs& _mouse ){
		glm::vec2 normalizedMouse = glm::vec2 (_mouse.x / (float)ofGetWindowWidth(), _mouse.y / (float)ofGetWindowHeight());
		if (!mousePositionsSet) {
			mousePositionsSet = true;
			mousePositions[mps] = normalizedMouse;
		}
		mps = !mps;
		mousePositions[mps] = normalizedMouse;
	}
	
	//--------------------------------------------------------------
	void ftActorFlow::reset() {
		ftFlow::reset();
		bMouseDragged = false;
		bFlowChanged = false;
	}

	void ftActorFlow::setActorManager(shared_ptr<ActorManager> actorManagerPtr) {
		actorManager = actorManagerPtr;
	}
	
}

