#include "Controller.h"
#include "ofPixels.h"
#include "ofGraphics.h"
#include "vrEvents.h"

namespace ofxOpenVrUtil {
	Controller::Controller(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role, ofPtr<Model> model) :
		deviceIndex(deviceIndex), role(role), model(model)
	{}
	void Controller::draw() {
		ofPushMatrix();
		ofMultMatrix(transform);
		model->tex.bind();
		model->mesh.draw();
		model->tex.unbind();
		ofPopMatrix();
	}
	
	void ControllerManager::setup(vr::IVRSystem* vrSys) {
		this->vrSys = vrSys;
	}
	bool ControllerManager::hasDevice(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role) const {
		if (!hasDevices()) return false;
		for (auto& c : controllers) {
			if (c.second->deviceIndex == deviceIndex && c.second->role == role) {
				return true;
			}
		}
		return false;
	}
	void ControllerManager::addDevice(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role) {
		if (models.find(role) == models.end()) {
			// If don't have a certain model for ControllerRole, let us load model for it
			loadModel(deviceIndex, role);
		}
		controllers[deviceIndex] = std::make_shared<Controller>(deviceIndex, role, models[role]);
	}

	void ControllerManager::update() {
		for (auto c : controllers) {
			auto cc = c.second;
			vr::VRControllerState_t controllerState;
			vrSys->GetControllerState(cc->deviceIndex, &controllerState, sizeof(vr::VRControllerState_t));

			if (cc->pad.pressed || cc->pad.touched) {
				auto axis = controllerState.rAxis[0]; // vr::k_eControllerAxis_TrackPad
				glm::vec2 a(axis.x, axis.y);
				if (cc->pad.axis != a) {
					cc->pad.axis = a;
					ofLogVerbose("controller") << "touchpad\t axis: (" << a.x << ", " << a.y << ")";
					Event::AxisMove e{ a, cc->deviceIndex, cc->role, cc->getPosition() };
					ofNotifyEvent(Event::onPadUnpressed, e);
				}
			}

			if (cc->trigger.pressed || cc->trigger.touched) {
				auto axis = controllerState.rAxis[1]; // vr::k_eControllerAxis_Trigger
				glm::vec2 a(axis.x, axis.y);
				if (cc->trigger.axis != a) {
					cc->trigger.axis = a;
					ofLogVerbose("controller") << "trigger\t axis: (" << a.x << ")";
					Event::AxisMove e{ a, cc->deviceIndex, cc->role, cc->getPosition() };
					ofNotifyEvent(Event::onTriggerUnpressed, e);
				}
			}

			if (cc->joyStick.pressed || cc->joyStick.touched) {
				auto axis = controllerState.rAxis[2]; // vr::k_eControllerAxis_Joystick
				glm::vec2 a(axis.x, axis.y);
				if (cc->joyStick.axis != a) {
					cc->joyStick.axis = a;
					ofLogVerbose("controller") << "joystick\t axis: (" << a.x << ")";
					Event::AxisMove e{ a, cc->deviceIndex, cc->role, cc->getPosition() };
					ofNotifyEvent(Event::onJoyStickUnpressed, e);
				}
			}

		}
	}

	void ControllerManager::handleInput(const vr::VREvent_t& ev) {
		// https://github.com/ValveSoftware/openvr/wiki/VREvent_t
		// https://github.com/ValveSoftware/openvr/wiki/IVRSystem::GetControllerState

		vr::VRControllerState_t controllerState;
		vrSys->GetControllerState(ev.trackedDeviceIndex, &controllerState, sizeof(vr::VRControllerState_t));

		std::string evTypeStr;
		switch (ev.eventType) {
		case vr::VREvent_ButtonPress: evTypeStr = "pressStart"; break;
		case vr::VREvent_ButtonUnpress: evTypeStr = "pressEnd"; break;
		case vr::VREvent_ButtonTouch: evTypeStr = "touchStart"; break;
		case vr::VREvent_ButtonUntouch: evTypeStr = "touchEnd"; break;
		default: evTypeStr = "other(" + ofToString(ev.eventType) + ")"; break;
		}

		if (lastPacketNum == controllerState.unPacketNum) {
			// ignore event because packet num is same as last
			return;
		}

		lastPacketNum = controllerState.unPacketNum;

		uint64_t mask = vr::ButtonMaskFromId(vr::EVRButtonId(ev.data.controller.button));
		bool pressed = mask & controllerState.ulButtonPressed;
		bool touched = mask & controllerState.ulButtonTouched;

		if (controllers.find(ev.trackedDeviceIndex) == controllers.end()) return;
		auto c = controllers[ev.trackedDeviceIndex];
		
		std::string debugStr = std::string(pressed ? "pressed" : "unpressed") + ", " + (touched ? "touched" : "untouched");

		switch (ev.data.controller.button) {
		case vr::k_EButton_SteamVR_Touchpad:
		{
			auto axis = controllerState.rAxis[0]; // vr::k_eControllerAxis_TrackPad
			ofLogVerbose(evTypeStr) << "touchpad\t axis: (" << axis.x << ", " << axis.y << "), " << debugStr;
			c->pad.axis = glm::vec2(axis.x, axis.y);
			c->pad.pressed = pressed;
			c->pad.touched = touched;

			Event::AxisMove e{ c->pad.axis, c->deviceIndex, c->role, c->getPosition() };
			if (ev.eventType == vr::VREvent_ButtonPress) ofNotifyEvent(Event::onPadPressed, e);
			else if (ev.eventType == vr::VREvent_ButtonUnpress) ofNotifyEvent(Event::onPadUnpressed, e);
			else if (ev.eventType == vr::VREvent_ButtonTouch) ofNotifyEvent(Event::onPadTouched, e);
			else if (ev.eventType == vr::VREvent_ButtonUntouch) ofNotifyEvent(Event::onPadUntouched, e);

		} break;
		case vr::k_EButton_SteamVR_Trigger:
		{			
			auto axis = controllerState.rAxis[1]; // vr::k_eControllerAxis_Trigger
			ofLogVerbose(evTypeStr) << "trigger\t axis: (" << axis.x << "), " << debugStr;
			c->trigger.axis = glm::vec2(axis.x, axis.y);
			c->trigger.pressed = pressed;
			c->trigger.touched = touched;

			Event::AxisMove e{ c->pad.axis, c->deviceIndex, c->role, c->getPosition() };
			if (ev.eventType == vr::VREvent_ButtonPress) ofNotifyEvent(Event::onTriggerPressed, e);
			else if (ev.eventType == vr::VREvent_ButtonUnpress) ofNotifyEvent(Event::onTriggerUnpressed, e);
			else if (ev.eventType == vr::VREvent_ButtonTouch) ofNotifyEvent(Event::onTriggerTouched, e);
			else if (ev.eventType == vr::VREvent_ButtonUntouch) ofNotifyEvent(Event::onTriggerUntouched, e);

		} break;
		case vr::k_EButton_IndexController_JoyStick:
		{
			auto axis = controllerState.rAxis[2]; // vr::k_eControllerAxis_Joystick
			ofLogVerbose(evTypeStr) << "joystick\t axis: (" << axis.x << ", " << axis.y << "), " << debugStr;
			c->joyStick.axis = glm::vec2(axis.x, axis.y);
			c->joyStick.pressed = pressed;
			c->joyStick.touched = touched;

			Event::AxisMove e{ c->pad.axis, c->deviceIndex, c->role, c->getPosition() };
			if (ev.eventType == vr::VREvent_ButtonPress) ofNotifyEvent(Event::onJoyStickPressed, e);
			else if (ev.eventType == vr::VREvent_ButtonUnpress) ofNotifyEvent(Event::onJoyStickUnpressed, e);
			else if (ev.eventType == vr::VREvent_ButtonTouch) ofNotifyEvent(Event::onJoyStickTouched, e);
			else if (ev.eventType == vr::VREvent_ButtonUntouch) ofNotifyEvent(Event::onJoyStickUntouched, e);

		} break;
		case vr::k_EButton_A:
		{
			ofLogVerbose(evTypeStr) << "button_a\t " << debugStr;
			c->buttons[vr::k_EButton_A].pressed = pressed;
			c->buttons[vr::k_EButton_A].touched = touched;

			Event::ButtonInfo e{ vr::k_EButton_A, c->deviceIndex, c->role, c->getPosition() };
			if (ev.eventType == vr::VREvent_ButtonPress) ofNotifyEvent(Event::onButtonPressed, e);
			else if (ev.eventType == vr::VREvent_ButtonUnpress) ofNotifyEvent(Event::onButtonUnpressed, e);

		} break;
		case vr::k_EButton_Grip:
		{
			ofLogVerbose(evTypeStr) << "button_grip\t " << debugStr;
			c->buttons[vr::k_EButton_Grip].pressed = pressed;
			c->buttons[vr::k_EButton_Grip].touched = touched;

			Event::ButtonInfo e{ vr::k_EButton_Grip, c->deviceIndex, c->role, c->getPosition() };
			if (ev.eventType == vr::VREvent_ButtonPress) ofNotifyEvent(Event::onButtonPressed, e);
			else if (ev.eventType == vr::VREvent_ButtonUnpress) ofNotifyEvent(Event::onButtonUnpressed, e);

		} break;
		default:
		{
			ofLogVerbose(evTypeStr) << "button( " << ev.data.controller.button << " )\t " << debugStr;
			c->buttons[ev.data.controller.button].pressed = pressed;
			c->buttons[ev.data.controller.button].touched = touched;

			Event::ButtonInfo e{ (vr::EVRButtonId)ev.data.controller.button, c->deviceIndex, c->role, c->getPosition() };
			if (ev.eventType == vr::VREvent_ButtonPress) ofNotifyEvent(Event::onButtonPressed, e);
			else if (ev.eventType == vr::VREvent_ButtonUnpress) ofNotifyEvent(Event::onButtonUnpressed, e);
		}
		}
	}

	void ControllerManager::loadModel(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role) {
		
		std::string modelName = getPropString(vrSys, deviceIndex, vr::Prop_RenderModelName_String);
		vr::RenderModel_t* renderModel = nullptr;

		// Polling in every 1ms to async function 
		vr::EVRRenderModelError error;
		while (true) {
			error = vr::VRRenderModels()->LoadRenderModel_Async(modelName.c_str(), &renderModel);
			if (error != vr::VRRenderModelError_Loading) break;
			ofSleepMillis(1);
		}

		if (error != vr::VRRenderModelError_None) {
			ofLogError(__FUNCTION__) << "Unable to create model from render model";
			vr::VRRenderModels()->FreeRenderModel(renderModel);
			return;
		}

		vr::RenderModel_TextureMap_t* renderModelTex;
		while (true) {
			error = vr::VRRenderModels()->LoadTexture_Async(renderModel->diffuseTextureId, &renderModelTex);
			if (error != vr::VRRenderModelError_Loading) break;
			ofSleepMillis(1);
		}

		if (error != vr::VRRenderModelError_None) {
			ofLogError(__FUNCTION__) << "Unable to load render texture of: " << modelName;
			vr::VRRenderModels()->FreeRenderModel(renderModel);
			vr::VRRenderModels()->FreeTexture(renderModelTex);
			return;
		}

		ofPtr<Model> m = std::make_shared<Model>();
		m->tex.allocate(renderModelTex->unWidth, renderModelTex->unHeight, GL_RGBA8, false);
		m->tex.loadData(renderModelTex->rubTextureMapData, renderModelTex->unWidth, renderModelTex->unHeight, GL_RGBA);

		m->mesh.setMode(OF_PRIMITIVE_TRIANGLES);
		for (int i = 0; i < renderModel->unVertexCount; i++) {
			auto& v = renderModel->rVertexData[i];
			m->mesh.addVertex(toGlm(v.vPosition));
			m->mesh.addNormal(toGlm(v.vNormal));
			m->mesh.addTexCoord(toGlm(v.rfTextureCoord));
		}
		for (int i = 0; i < renderModel->unTriangleCount; i++) {
			m->mesh.addIndex(renderModel->rIndexData[i * 3 + 0]);
			m->mesh.addIndex(renderModel->rIndexData[i * 3 + 1]);
			m->mesh.addIndex(renderModel->rIndexData[i * 3 + 2]);
		}

		vr::VRRenderModels()->FreeRenderModel(renderModel);
		vr::VRRenderModels()->FreeTexture(renderModelTex);
		models[role] = m;
		
		ofLogNotice(__FUNCTION__) << "Model ( name: " << modelName << ", vertex: " << m->mesh.getNumVertices() << " ) was loaded!";
	}
}