#include "ofxOpenVrUtil.h"
#include "Utils.h"
#include "ofGraphics.h"

namespace ofxOpenVrUtil {
	Interface::Interface() :
		vrSys(nullptr),  systemName("No Driver"), modelNumber("No Display")
	{}

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

		// TODO: Prepare RenderModel

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
		// TODO: handle input

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

		// TODO: 

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

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		hmd.getHiddenMesh(eye).draw();

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
	}

	void Interface::updateTrackedDeviceMatrix() {

		auto e = vr::VRCompositor()->WaitGetPoses(trackedDevivePose.data(), (uint32_t)trackedDevivePose.size(), NULL, 0);
		if (e != vr::VRCompositorError_None) {
			ofLogError(__FUNCTION__) << "Can't track device poses.";
		}
		
		// Loop through tracked devices. Max count is 64 (vr::k_unMaxTrackedDeviceCount)
		// Index of 0 is always HMD (vr::k_unTrackedDeviceIndex_Hmd)
		int validPoseCount = 0;
		for (int i = 0; i < trackedDevivePose.size(); i++) {
			if (trackedDevivePose[i].bPoseIsValid) {
				validPoseCount++;

				// trackedDeviceMatrix[i] = toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking);

				switch (vrSys->GetTrackedDeviceClass(i)) {
				case vr::TrackedDeviceClass_Controller: {
					auto controllerRole = vrSys->GetControllerRoleForTrackedDeviceIndex(i);
					if (controllers.hasDevice(i, controllerRole)) {
						controllers.get()[i]->setTransformMatrix(toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking));
					} else {
						controllers.addDevice(i, controllerRole);
					}
				} break;
				case vr::TrackedDeviceClass_HMD: {
					hmd.setTransformMatrix(toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking));
				} break;
				case vr::TrackedDeviceClass_Invalid: break;
				case vr::TrackedDeviceClass_GenericTracker: break;
				case vr::TrackedDeviceClass_TrackingReference: break;
				default: break;
				}


			} else {
				continue;
			}
		
		}
		
		/*if (trackedDevivePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
			hmd.setTransformMatrix(trackedDeviceMatrix[vr::k_unTrackedDeviceIndex_Hmd]);
		}*/

	}

	void Interface::handleInput() {
		vr::VREvent_t ev;
		while (vrSys->PollNextEvent(&ev, sizeof(vr::VREvent_t))) {
			
			// processEvent(ev);
		}
		


	}

}
