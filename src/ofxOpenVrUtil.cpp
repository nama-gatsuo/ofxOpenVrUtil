#include "ofxOpenVrUtil.h"
#include "vrUtils.h"
#include "ofGraphics.h"
#include "of3dGraphics.h"
#include "of3dUtils.h"
#include "vrEvents.h"

namespace ofxOpenVrUtil {
	Interface::Interface() :
		vrSys(nullptr), systemName("No Driver"), modelNumber("No Display") {
	}

	void Interface::setup() {

		// Init VR
		vr::EVRInitError e = vr::VRInitError_None;
		vrSys = vr::VR_Init(&e, vr::VRApplication_Scene);
		if (e != vr::VRInitError_None) {
			ofLogError(__FUNCTION__) << vr::VR_GetVRInitErrorAsEnglishDescription(e);
			return;
		}

		// Get Property strings
		systemName = getPropString(vrSys, vr::Prop_TrackingSystemName_String);
		modelNumber = getPropString(vrSys, vr::Prop_ModelNumber_String);

		if (!vr::VRCompositor()) {
			ofLogError(__FUNCTION__) << "Compositor initialization failed.";
			return;
		}

		hmd.setup(vrSys);
		controllers.setup(vrSys);
		trackedCam.setup(vrSys);

		trackedDevivePose.resize(vr::k_unMaxTrackedDeviceCount);
		trackedDeviceMatrix.resize(vr::k_unMaxTrackedDeviceCount);

		ofLogNotice(__FUNCTION__) << "VR System: " << systemName;
		ofLogNotice(__FUNCTION__) << "VR Model: " << modelNumber;

	}

	void Interface::update() {

		updateTrackedDeviceMatrix();
		handleInput();
		controllers.update();

		if (trackedCam.isStreaming()) trackedCam.update();

	}

	void Interface::submit(const ofTexture& tex, vr::EVREye eye) {
		vr::Texture_t vrTex = { (void*)(uintptr_t)(tex.getTextureData().textureID), vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(eye, &vrTex);
	}

	void Interface::exit() {
		vr::VR_Shutdown();
		vrSys = nullptr;
	}

	void Interface::beginEye(vr::EVREye eye) {
		ofPushMatrix();
		ofPushView();
		ofEnableDepthTest();

		ofSetMatrixMode(OF_MATRIX_PROJECTION);
		ofLoadMatrix(hmd.getProjectionMatrix(eye));
		ofSetMatrixMode(OF_MATRIX_MODELVIEW);
		ofLoadMatrix(hmd.getViewMatrix(eye));
	}

	void Interface::endEye() {
		ofDisableDepthTest();
		ofPopMatrix();
		ofPopView();
	}

	void Interface::applyEyeStencil(vr::Hmd_Eye eye) {
		glClear(GL_STENCIL_BUFFER_BIT);
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		ofSetColor(0, 0);
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should update the stencil buffer
		glStencilMask(0xFF); // enable writing to the stencil buffer
		hmd.getHiddenMesh(eye).draw();

		ofSetColor(255);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00); // disable writing to the stencil buffer
	}

	void Interface::debugDraw() {
		ofPushMatrix();
		ofMultMatrix(hmd.getTransformMatrix());
		ofDrawAxis(0.2f);
		ofDrawBox(0.08f);
		ofPopMatrix();

		for (auto& c : controllers.get()) {
			ofPushMatrix();
			ofMultMatrix(c.second->getTransform());
			ofDrawAxis(0.14f);
			ofPopMatrix();
		}
	}

	void Interface::updateTrackedDeviceMatrix() {

		auto e = vr::VRCompositor()->WaitGetPoses(trackedDevivePose.data(), (uint32_t)trackedDevivePose.size(), NULL, 0);
		if (e != vr::VRCompositorError_None) {
			ofLogError(__FUNCTION__) << "Can't track device poses.";
		}

		// Loop through tracked devices. Max count is 64 (vr::k_unMaxTrackedDeviceCount)
		// Index of 0 is always HMD (vr::k_unTrackedDeviceIndex_Hmd)
		int validPoseCount = 0;
		for (vr::TrackedDeviceIndex_t i = 0; i < trackedDevivePose.size(); i++) {
			if (trackedDevivePose[i].bPoseIsValid) {
				validPoseCount++;

				// trackedDeviceMatrix[i] = toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking);

				switch (vrSys->GetTrackedDeviceClass(i)) {
				case vr::TrackedDeviceClass_Controller:
				{
					auto controllerRole = vrSys->GetControllerRoleForTrackedDeviceIndex(i);
					if (controllers.hasDevice(i, controllerRole)) {
						glm::mat4 m = toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking);
						auto c = controllers.get()[i];

						if (c->getTransform() != m) {
							c->setTransformMatrix(toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking));

							Event::TrackedDeviceMove e{ c->getTransform(), controllerRole, i };
							ofNotifyEvent(Event::onControllerMove, e);
						}

					} else {
						controllers.addDevice(i, controllerRole);
					}
				} break;
				case vr::TrackedDeviceClass_HMD:
				{
					glm::mat4 m = toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking);
					if (hmd.pose != m) {
						hmd.pose = m;

						Event::TrackedDeviceMove e{ hmd.getTransformMatrix(), vr::TrackedControllerRole_Invalid, i };
						ofNotifyEvent(Event::onHmdMove, e);
					}

				} break;
				case vr::TrackedDeviceClass_Invalid: break;
				case vr::TrackedDeviceClass_GenericTracker: {
					auto it = std::find_if(trackers.begin(), trackers.end(), [&](ofPtr<Tracker> tracker) { return tracker->trackedDeviceId == i; });
					if (it == trackers.end()) {
						ofPtr<Tracker> tracker = std::make_shared<Tracker>();
						tracker->trackedDeviceId = i;
						tracker->transform = toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking);
						trackers.push_back(std::move(tracker));
					} else {
						(*it)->transform = toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking);
					}
				} break;
				case vr::TrackedDeviceClass_TrackingReference: {
					auto it = std::find_if(trackerRefs.begin(), trackerRefs.end(), [&](ofPtr<TrackerReference> trackerRef) { return trackerRef->trackedDeviceId == i; });
					if (it == trackerRefs.end()) {
						ofPtr<TrackerReference> trackerRef = std::make_shared<TrackerReference>();
						trackerRef->trackedDeviceId = i;
						trackerRef->transform = toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking);
						trackerRefs.push_back(std::move(trackerRef));
					} else {
						(*it)->transform = toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking);
					}
				} break;
				default: break;
				}


			} else {
				continue;
			}

		}
		trackeDeviceNum = validPoseCount;

	}

	void Interface::handleInput() {
		vr::VREvent_t ev;
		while (vrSys->PollNextEvent(&ev, sizeof(vr::VREvent_t))) {

			// Pick up only controller event for now
			if (vrSys->GetTrackedDeviceClass(ev.trackedDeviceIndex) == vr::TrackedDeviceClass_Controller) {
				controllers.handleInput(ev);
			}

		}

	}


	NoHmdInterface::NoHmdInterface() {}

	NoHmdInterface::~NoHmdInterface() {}

	void NoHmdInterface::setup() {

		vr::EVRInitError e = vr::VRInitError_None;
		// Initialize without activating VR
		vrSys = vr::VR_Init(&e, vr::VRApplication_Background);
		if (e != vr::VRInitError_None) {
			ofLogError(__FUNCTION__) << vr::VR_GetVRInitErrorAsEnglishDescription(e);
		}

		controllers.setup(vrSys);

		trackedDevivePose.resize(vr::k_unMaxTrackedDeviceCount);
	}

	void NoHmdInterface::update() {
		vrSys->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseRawAndUncalibrated, 0, trackedDevivePose.data(), vr::k_unMaxTrackedDeviceCount);

		int validPoseCount = 0;
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			if (trackedDevivePose[i].bPoseIsValid) {
				switch (vrSys->GetTrackedDeviceClass(i)) {
				case vr::TrackedDeviceClass_Controller:
				{
					auto controllerRole = vrSys->GetControllerRoleForTrackedDeviceIndex(i);
					if (controllers.hasDevice(i, controllerRole)) {
						glm::mat4 m = toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking);
						auto c = controllers.get()[i];

						if (c->getTransform() != m) {
							c->setTransformMatrix(toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking));

							Event::TrackedDeviceMove e{ c->getTransform(), controllerRole, i };
							ofNotifyEvent(Event::onControllerMove, e);
						}

					} else {
						controllers.addDevice(i, controllerRole);
					}
				} break;
				case vr::TrackedDeviceClass_HMD: break;
				case vr::TrackedDeviceClass_GenericTracker: {
					
					auto it = std::find_if(trackers.begin(), trackers.end(), [&](ofPtr<Tracker> tracker) { return tracker->trackedDeviceId == i; });
					if (it == trackers.end()) {
						ofPtr<Tracker> tracker = std::make_shared<Tracker>();
						tracker->trackedDeviceId = i;
						tracker->transform = toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking);
						trackers.push_back(std::move(tracker));
					} else {
						(*it)->transform = toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking);
					}

				} break;
				case vr::TrackedDeviceClass_TrackingReference: {
					auto it = std::find_if(trackerRefs.begin(), trackerRefs.end(), [&](ofPtr<TrackerReference> trackerRef) { return trackerRef->trackedDeviceId == i; });
					if (it == trackerRefs.end()) {
						ofPtr<TrackerReference> trackerRef = std::make_shared<TrackerReference>();
						trackerRef->trackedDeviceId = i;
						trackerRef->transform = toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking);
						trackerRefs.push_back(std::move(trackerRef));
					} else {
						(*it)->transform = toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking);
					}
				} break;
				case vr::TrackedDeviceClass_Invalid: break;
				default: break;
				}

			}
		}
		trackeDeviceNum = validPoseCount;
	}

	void NoHmdInterface::exit() {}

}
