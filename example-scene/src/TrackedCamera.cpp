#include "TrackedCamera.h"

namespace ofxOpenVrUtil {
	TrackedCamera::TrackedCamera(vr::IVRSystem* vrSys) : vrSys(vrSys) {}
	TrackedCamera::~TrackedCamera() {
		vrSys = nullptr;
		trackedCamera = nullptr;
	}

	bool TrackedCamera::open() {
		if (!vrSys) ofLogError(__FUNCTION__) << "IVRSystem is not initialized.";

		trackedCamera = vr::VRTrackedCamera();
		if (!trackedCamera) {
			ofLogError(__FUNCTION__) << "Unable to get Tracked Camera interface.";
			return false;
		}

		bool bHasCamera = false;
		vr::EVRTrackedCameraError e = trackedCamera->HasCamera(vr::k_unTrackedDeviceIndex_Hmd, &bHasCamera);

		if (e != vr::VRTrackedCameraError_None || !bHasCamera) {
			ofLogError() << "No Tracked Camera Available! ( " << trackedCamera->GetCameraErrorNameFromEnum(e) << ")";
			return false;
		}

		return false;
	}

	bool TrackedCamera::start() {

		uint32_t size = 0;
		// Allocate for camera frame buffer requirements
		vr::EVRTrackedCameraError e = trackedCamera->GetCameraFrameSize(
			vr::k_unTrackedDeviceIndex_Hmd,
			vr::VRTrackedCameraFrameType_Undistorted,
			&frameWidth, &frameHeight, &size
		);

		if (e != vr::VRTrackedCameraError_None) {
			ofLogError() << "GetCameraFrameBounds() Failed!";
			return false;
		}
		ofLogNotice(__FUNCTION__) << "Tracked Cam input size: " << frameWidth << " x " << frameHeight;

		if (size != 0 && size != bufferSize) {
			delete[] rawFrameBuffer;
			bufferSize = size;
			rawFrameBuffer = new uint8_t[bufferSize];
			memset(rawFrameBuffer, 0, bufferSize);
		}

		trackedCamera->AcquireVideoStreamingService(vr::k_unTrackedDeviceIndex_Hmd, &trackedCameraHandle);
		if (trackedCameraHandle == INVALID_TRACKED_CAMERA_HANDLE) {
			ofLogError(__FUNCTION__) << "AcquireVideoStreamingService() Failed!";
			return false;
		}

		tex[vr::Eye_Left].allocate(frameWidth, frameHeight / 2, GL_RGB);
		tex[vr::Eye_Right].allocate(frameWidth, frameHeight / 2, GL_RGB);

		lastFrameCount = 0;

		return true;

	}

	bool TrackedCamera::stop() {
		trackedCamera->ReleaseVideoStreamingService(trackedCameraHandle);
		trackedCameraHandle = INVALID_TRACKED_CAMERA_HANDLE;

		tex[vr::Eye_Left].clear();
		tex[vr::Eye_Right].clear();

		return true;
	}

	void TrackedCamera::update() {
		
		vr::CameraVideoStreamFrameHeader_t frameHeader;
		vr::EVRTrackedCameraError nCameraError = trackedCamera->GetVideoStreamFrameBuffer(trackedCameraHandle, vr::VRTrackedCameraFrameType_Undistorted, nullptr, 0, &frameHeader, sizeof(frameHeader));
		if (nCameraError != vr::VRTrackedCameraError_None) return;
		if (frameHeader.nFrameSequence == lastFrameCount) {
			// frame hasn't changed yet, nothing to do
			return;
		}

		nCameraError = trackedCamera->GetVideoStreamFrameBuffer(trackedCameraHandle, vr::VRTrackedCameraFrameType_Undistorted, rawFrameBuffer, bufferSize, &frameHeader, sizeof(frameHeader));
		if (nCameraError != vr::VRTrackedCameraError_None) {
			return;
		}

		lastFrameCount = frameHeader.nFrameSequence;

		for (int i = 0; i < 2; i++) {
			if (!pix[i].isAllocated()) {
				pix[i].allocate(frameWidth, frameHeight / 2, 4);
			}
			if (i == vr::Hmd_Eye::Eye_Left) {
				pix[i].setFromPixels(
					rawFrameBuffer + bufferSize / 2, frameWidth, frameHeight / 2, 4
				);
			} else {
				pix[i].setFromPixels(rawFrameBuffer, frameWidth, frameHeight / 2, 4);
			}
			
		}

	}


}