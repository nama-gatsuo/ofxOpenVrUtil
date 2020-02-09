#pragma once
#include "ofVboMesh.h"
#include "ofTexture.h"
#include "openvr.h"
#include "Utils.h"
#include "ofTypes.h"

#include <unordered_map>

namespace ofxOpenVrUtil {

	struct Model {
		ofVboMesh mesh;
		ofTexture tex;
	};

	class Controller {
	public:
		Controller(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role, ofPtr<Model> model);
		bool isConnected() { return false; }
		void draw();
		
		const vr::TrackedDeviceIndex_t deviceIndex; // just uint32_t 0 - 64
		const vr::ETrackedControllerRole role;
		void setTransformMatrix(const glm::mat4& t) {
			transform = t;
		}

	private:
		ofPtr<Model> model;
		glm::mat4 transform;
	};

	class ControllerManager {
	public:
		void setup(vr::IVRSystem* vrSys);
		size_t size() const { return controllers.size(); }
		bool hasDevices() const { return !controllers.empty(); }
		bool hasDevice(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role) const;
		void addDevice(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role);
		std::unordered_map<vr::TrackedDeviceIndex_t, ofPtr<Controller>>& get() { return controllers; }
	private:
		void loadModel(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role);

		vr::IVRSystem* vrSys;
		std::unordered_map<vr::TrackedDeviceIndex_t, ofPtr<Controller>> controllers;
		std::unordered_map<vr::ETrackedControllerRole, ofPtr<Model>> models;
	};

}