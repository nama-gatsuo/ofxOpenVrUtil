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

		const glm::mat4& getTransform() const { return transform; }
		glm::vec3 getPosition() const { return glm::vec3(transform[3]); }

		void setTransformMatrix(const glm::mat4& t) {
			transform = t;
		}

		struct ButtonState {
			bool pressed = false;
			bool touched = false;
		};
		struct AxisState {
			glm::vec2 axis;
			bool pressed = false;
			bool touched = false;
		};

	private:
		friend class ControllerManager;
		const vr::TrackedDeviceIndex_t deviceIndex; // just uint32_t 0 - 64
		const vr::ETrackedControllerRole role;

		ofPtr<Model> model;
		glm::mat4 transform;

		std::unordered_map<int, ButtonState> buttons;
		AxisState trigger, pad, joyStick;

	};

	class ControllerManager {
	public:
		void setup(vr::IVRSystem* vrSys);
		size_t size() const { return controllers.size(); }
		bool hasDevices() const { return !controllers.empty(); }
		bool hasDevice(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role) const;
		void addDevice(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role);
		std::unordered_map<vr::TrackedDeviceIndex_t, ofPtr<Controller>>& get() { return controllers; }

		void update();
		void handleInput(const vr::VREvent_t& ev);

	private:
		void loadModel(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role);

		vr::IVRSystem* vrSys;
		std::unordered_map<vr::TrackedDeviceIndex_t, ofPtr<Controller>> controllers;
		std::unordered_map<vr::ETrackedControllerRole, ofPtr<Model>> models;

		int lastPacketNum;
	};

}