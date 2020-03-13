#pragma once
#include "vrEvents.h"

namespace ofxOpenVrUtil {

	ofEvent<Event::TrackedDeviceMove> Event::onControllerMove;
	ofEvent<Event::TrackedDeviceMove> Event::onHmdMove;

	ofEvent<Event::AxisMove> Event::onTriggerPressed;
	ofEvent<Event::AxisMove> Event::onTriggerUnpressed;
	ofEvent<Event::AxisMove> Event::onTriggerTouched;
	ofEvent<Event::AxisMove> Event::onTriggerUntouched;
	ofEvent<Event::AxisMove> Event::onTriggerUpdate;

	ofEvent<Event::AxisMove> Event::onPadPressed;
	ofEvent<Event::AxisMove> Event::onPadUnpressed;
	ofEvent<Event::AxisMove> Event::onPadTouched;
	ofEvent<Event::AxisMove> Event::onPadUntouched;
	ofEvent<Event::AxisMove> Event::onPadUpdate;

	ofEvent<Event::AxisMove> Event::onJoyStickPressed;
	ofEvent<Event::AxisMove> Event::onJoyStickUnpressed;
	ofEvent<Event::AxisMove> Event::onJoyStickTouched;
	ofEvent<Event::AxisMove> Event::onJoyStickUntouched;
	ofEvent<Event::AxisMove> Event::onJoyStickUpdate;

	ofEvent<Event::ButtonInfo> Event::onButtonPressed;
	ofEvent<Event::ButtonInfo> Event::onButtonUnpressed;

};