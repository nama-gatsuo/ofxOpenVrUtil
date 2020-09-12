#pragma once

#include <functional>
#include "openvr.h"
#include "ofParameter.h"
#include "TrackedCamera.h"
#include "Hmd.h"
#include "Controller.h"

namespace ofxOpenVrUtil {

	struct Tracker {
		glm::mat4 transform;
		vr::TrackedDeviceIndex_t trackedDeviceId;
	};

	struct TrackerReference {
		glm::mat4 transform;
		vr::TrackedDeviceIndex_t trackedDeviceId;
	};

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
		std::unordered_map<vr::TrackedDeviceIndex_t, ofPtr<Controller>>& getControllers() { return controllers.get(); }
		bool hasControllers() const { return controllers.hasDevices(); }

		/* Apply HMD ModelView transformation per eye */
		void beginEye(vr::EVREye eye);
		void endEye();

		/*
		Apply stencil test of hidden area.
		VR display has the lens distortion thus has unnecessary pixels from frame buffer rectangle.
		This function enables to avoid pixel to be renderred with Hidden Area Mesh stencil.
		Only HTC Vive has Hidden Area Mesh stencil, Oculus doesn't.
		*/
		void applyEyeStencil(vr::Hmd_Eye eye);

		void debugDraw();

		int getNumTrackedDevice() const { return trackeDeviceNum; }

	private:
		void updateTrackedDeviceMatrix();
		void handleInput();

		ofParameterGroup group;

		vr::IVRSystem* vrSys;
		std::string systemName;
		std::string modelNumber;

		TrackedCamera trackedCam;
		Hmd hmd;
		ControllerManager controllers;

		std::vector<glm::mat4> trackedDeviceMatrix;
		std::vector<vr::TrackedDevicePose_t> trackedDevivePose;
		std::vector<ofPtr<Tracker>> trackers;
		std::vector<ofPtr<TrackerReference>> trackerRefs;

		int trackeDeviceNum;
	};

	class NoHmdInterface {
	public:
		NoHmdInterface();
		~NoHmdInterface();

		void setup();
		void update();
		void exit();

		const std::vector<ofPtr<Tracker>>& getTrackers() { return trackers; }
		const std::vector<ofPtr<TrackerReference>>& getTrackerRefs() { return trackerRefs; }
		int getNumTrackedDevice() const { return trackeDeviceNum; }

	private:
		vr::IVRSystem* vrSys;

		std::vector<vr::TrackedDevicePose_t> trackedDevivePose;
		int trackeDeviceNum;

		ControllerManager controllers;
		std::vector<ofPtr<Tracker>> trackers;
		std::vector<ofPtr<TrackerReference>> trackerRefs;
	};

}