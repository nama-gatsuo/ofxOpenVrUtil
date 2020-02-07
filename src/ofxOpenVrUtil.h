#pragma once

#include <functional>
#include "openvr.h"
#include "ofParameter.h"
#include "TrackedCamera.h"
#include "Hmd.h"

namespace ofxOpenVrUtil {
	

	class Interface {
	public:
		Interface();
		~Interface() {}

		void setup();
		void update();
		void submit(const ofTexture& tex, vr::EVREye eye);

		void exit();

		TrackedCamera& getTrackedCamera() { return trackedCam; }
		Hmd& getHmd() { return hmd; }

		void beginEye(vr::EVREye eye);
		void endEye();

	private:
		void updateTrackedDeviceMatrix();

		ofParameterGroup group;

		vr::IVRSystem* vrSys;
		std::string systemName;
		std::string modelNumber;

		TrackedCamera trackedCam;
		Hmd hmd;
		std::vector<glm::mat4> trackedDeviceMatrix;
		std::vector<vr::TrackedDevicePose_t> trackedDevivePose;

	};

}