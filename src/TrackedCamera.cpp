#include "TrackedCamera.h"
#include "Utils.h"
#include "ofGraphics.h"

namespace ofxOpenVrUtil {

	TrackedCamera::TrackedCamera() : vrSys(nullptr), trackedCamera(nullptr), bStreaming(false) {}
	TrackedCamera::~TrackedCamera() {
		if (bStreaming) stop();

		vrSys = nullptr;
		trackedCamera = nullptr;
	}

	void TrackedCamera::setup(vr::IVRSystem* vrSys) {
		this->vrSys = vrSys;

		if (!vrSys) ofLogError(__FUNCTION__) << "IVRSystem is not initialized.";

		trackedCamera = vr::VRTrackedCamera();
		if (!trackedCamera) {
			ofLogError(__FUNCTION__) << "Unable to get Tracked Camera interface.";
			return;
		}

		bool bHasCamera = false;
		vr::EVRTrackedCameraError e = trackedCamera->HasCamera(vr::k_unTrackedDeviceIndex_Hmd, &bHasCamera);

		if (e != vr::VRTrackedCameraError_None || !bHasCamera) {
			ofLogError(__FUNCTION__) << "No Tracked Camera Available! ( " << trackedCamera->GetCameraErrorNameFromEnum(e) << " )";
			return;
		}

		std::string camFirmwareDisc = getPropString(vrSys, vr::Prop_CameraFirmwareDescription_String);
		ofLogNotice(__FUNCTION__) << "Camera Firmware: " << camFirmwareDisc;
		{
			uint32_t size = 0;
			vr::EVRTrackedCameraError e = trackedCamera->GetCameraFrameSize(
				vr::k_unTrackedDeviceIndex_Hmd,
				vr::VRTrackedCameraFrameType_Undistorted,
				&frameWidth, &frameHeight, &size
			);

			if (e != vr::VRTrackedCameraError_None) {
				ofLogError(__FUNCTION__) << "GetCameraFrameBounds() Failed! ( " << trackedCamera->GetCameraErrorNameFromEnum(e) << " )";
				return;
			}
			ofLogNotice(__FUNCTION__) << "Tracked Cam input size: " << frameWidth << " x " << frameHeight << " (bufferSize: " << size << ")";
		}

		// Get inverse of camera projection matrix so that we can scale texture
		{
			
			for (int i = 0; i < 2; i++) {
				vr::HmdMatrix44_t mat;
				vr::EVRTrackedCameraError e = trackedCamera->GetCameraProjection(vr::k_unTrackedDeviceIndex_Hmd, i, vr::VRTrackedCameraFrameType_Undistorted, 1.f, -1.f, &mat);
				
				cam[i].invProj = glm::inverse(toGlm(mat));

				if (e != vr::VRTrackedCameraError_None) {
					ofLogError(__FUNCTION__) << "GetCameraProjection() - " << i << " Failed! ( " << trackedCamera->GetCameraErrorNameFromEnum(e) << " )";
				}
			}

		}
		
		// Get Camera to Hmd transform matrix which means local matrix
		// But currently camera transform doesn't work.
		// Only work with device transform
		{
			vr::ETrackedPropertyError e;
			std::vector<vr::HmdMatrix34_t> mat(2);
			vrSys->GetArrayTrackedDeviceProperty(
				vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_CameraToHeadTransforms_Matrix34_Array,
				vr::k_unHmdMatrix34PropertyTag, (void*)mat.data(), sizeof(vr::HmdMatrix34_t) * 2, &e
			);
			if (e != vr::TrackedProp_Success) {
				ofLogError(__FUNCTION__) << "Can't get Prop_CameraToHeadTransforms_Matrix34_Array ( " << vrSys->GetPropErrorNameFromEnum(e) << " )";
			}

			cam[0].transform = toGlm(mat[0]);
			cam[1].transform = toGlm(mat[1]);

		}

		bindTex.load("shader/tex");

		// This plane's aspect ratio is kinda magic number.
		// 1:1 didn't work with projection matrix, but 2:1 looks to work nicely.
		cam[0].rect = ofMesh::plane(2, 1, 2, 2);
		cam[1].rect = ofMesh::plane(2, 1, 2, 2);

		cam[vr::Eye_Left].rect.clearTexCoords();
		cam[vr::Eye_Left].rect.addTexCoord(glm::vec2(0, 1));
		cam[vr::Eye_Left].rect.addTexCoord(glm::vec2(1, 1));
		cam[vr::Eye_Left].rect.addTexCoord(glm::vec2(0, 0.5));
		cam[vr::Eye_Left].rect.addTexCoord(glm::vec2(1, 0.5));
		
		cam[vr::Eye_Right].rect.clearTexCoords();
		cam[vr::Eye_Right].rect.addTexCoord(glm::vec2(0, 0.5));
		cam[vr::Eye_Right].rect.addTexCoord(glm::vec2(1, 0.5));
		cam[vr::Eye_Right].rect.addTexCoord(glm::vec2(0, 0));
		cam[vr::Eye_Right].rect.addTexCoord(glm::vec2(1, 0));

	}

	bool TrackedCamera::start() {

		if (!trackedCamera) {
			ofLogError(__FUNCTION__) << "Tracked Camera is not opened yet.";
			return false;
		}
		
		trackedCamera->AcquireVideoStreamingService(vr::k_unTrackedDeviceIndex_Hmd, &trackedCameraHandle);
		if (trackedCameraHandle == INVALID_TRACKED_CAMERA_HANDLE) {
			ofLogError(__FUNCTION__) << "AcquireVideoStreamingService() Failed!";
			return false;
		}

		lastFrameCount = 0;
		bStreaming = true;

		return bStreaming;

	}

	bool TrackedCamera::stop() {
		trackedCamera->ReleaseVideoStreamingService(trackedCameraHandle);
		trackedCameraHandle = INVALID_TRACKED_CAMERA_HANDLE;

		bStreaming = false;

		return true;
	}

	void TrackedCamera::update() {
		
		vr::CameraVideoStreamFrameHeader_t frameHeader;
		vr::EVRTrackedCameraError e = trackedCamera->GetVideoStreamTextureGL(trackedCameraHandle, vr::VRTrackedCameraFrameType_Undistorted, nullptr, &frameHeader, sizeof(frameHeader));
		if (e != vr::VRTrackedCameraError_None) {
			ofLogError(__FUNCTION__) << trackedCamera->GetCameraErrorNameFromEnum(e);
			return;
		}
		if (frameHeader.nFrameSequence == lastFrameCount) {
			// frame hasn't changed yet, nothing to do
			return;
		}
		
		e = trackedCamera->GetVideoStreamTextureGL(trackedCameraHandle, vr::VRTrackedCameraFrameType_Undistorted, &texId, &frameHeader, sizeof(frameHeader));

		if (e != vr::VRTrackedCameraError_None) {
			ofLogError(__FUNCTION__) << trackedCamera->GetCameraErrorNameFromEnum(e);
			return;
		}
		deviceTransform = toGlm(frameHeader.trackedDevicePose.mDeviceToAbsoluteTracking);
		lastFrameCount = frameHeader.nFrameSequence;
		

	}

	void TrackedCamera::draw(vr::Hmd_Eye eye) {
		ofDisableDepthTest();
		bindTex.begin();
		glActiveTexture(GL_TEXTURE0 + 0);
		glClientActiveTexture(GL_TEXTURE0 + 0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texId);

		bindTex.setUniform1i("tex", 0);

		ofPushMatrix();
		ofMultMatrix(deviceTransform);
		ofMultMatrix(cam[eye].invProj); // Scaling texture
		
		cam[eye].rect.draw();
		ofPopMatrix();

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(0, 0);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);

		bindTex.end();
		ofEnableDepthTest();
	}

}