#pragma once

#include <functional>
#include "openvr.h"
#include "ofParameter.h"
#include "TrackedCamera.h"

namespace ofxOpenVrUtil {

	class Hmd {
	public:
		glm::mat4 getViewMatrix(vr::Hmd_Eye eye) const;
		glm::mat4 getProjectionMatrix(vr::Hmd_Eye eye) const;
		glm::mat4 getViewProjectionMatrix(vr::Hmd_Eye eye) const;

	private:

	};

	

	class Interface {
	public:
		Interface();
		~Interface();

		void setup();
		void exit();

	private:
		std::string getPropString(vr::TrackedDeviceProperty prop);

		ofParameterGroup group;
		vr::IVRSystem * vrSys;
		std::string systemName;
		std::string modelNumber;

		TrackedCamera trackedCam;

		ofParameter<bool> useCamera;
	};

}