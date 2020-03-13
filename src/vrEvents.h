#pragma once
#include "ofEvents.h"
#include "ofVectorMath.h"
#include "openvr.h"

namespace ofxOpenVrUtil {
	
	class Event {
	public:
	
		struct TrackedDeviceMove {
			glm::vec3 devicePos;
			vr::ETrackedControllerRole role;
			vr::TrackedDeviceIndex_t index;
		};

		static ofEvent<TrackedDeviceMove> onControllerMove;
		static ofEvent<TrackedDeviceMove> onHmdMove;

		struct AxisMove {
			glm::vec2 axis;
			vr::TrackedDeviceIndex_t index;
			vr::ETrackedControllerRole role;
			glm::vec3 devicePos;
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
			glm::vec3 devicePos;
		};

		static ofEvent<ButtonInfo> onButtonPressed;
		static ofEvent<ButtonInfo> onButtonUnpressed;
	
	};

	

};