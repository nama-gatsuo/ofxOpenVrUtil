#pragma once
#include "ofEvents.h"
#include "ofVectorMath.h"
#include "openvr.h"

namespace ofxOpenVrUtil {

	struct TrackedDeviceMove {
		glm::vec3 pos;
		vr::ETrackedDeviceClass deviceClass;
		vr::TrackedDeviceIndex_t index;
	};

	static ofEvent<TrackedDeviceMove> onControllerMove;
	static ofEvent<TrackedDeviceMove> onHmdMove;

	struct AxisMove {
		glm::vec2 axis;
		vr::TrackedDeviceIndex_t index;
		vr::ETrackedControllerRole role;
	};

	static ofEvent<AxisMove> onTriggerPressed;
	static ofEvent<AxisMove> onTriggerUnpressed;
	static ofEvent<AxisMove> onTriggerTouched;
	static ofEvent<AxisMove> onTriggerUntouched;
	static ofEvent<AxisMove> onTriggerUpdate;
	
	static ofEvent<AxisMove> onPadPressed;
	static ofEvent<AxisMove> onPadUnpressed;
	static ofEvent<AxisMove> onPadTouched;
	static ofEvent<AxisMove> onPadUntouched;
	static ofEvent<AxisMove> onPadUpdate;

	static ofEvent<AxisMove> onJoyStickPressed;
	static ofEvent<AxisMove> onJoyStickUnpressed;
	static ofEvent<AxisMove> onJoyStickTouched;
	static ofEvent<AxisMove> onJoyStickUntouched;
	static ofEvent<AxisMove> onJoyStickUpdate;

	struct ButtonInfo {
		vr::EVRButtonId buttonId;
		vr::TrackedDeviceIndex_t index;
		vr::ETrackedControllerRole role;
	};

	ofEvent<ButtonInfo> onButtonPressed;
	ofEvent<ButtonInfo> onButtonUnpressed;


};