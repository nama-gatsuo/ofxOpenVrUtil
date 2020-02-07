#include "TrackedCamera.h"
#include "Utils.h"

namespace ofxOpenVrUtil {

	TrackedCamera::TrackedCamera(vr::IVRSystem* vrSys) : vrSys(vrSys), bOpen(false), bStreaming(false) {}
	TrackedCamera::~TrackedCamera() {
		if (bStreaming) stop();
		if (bOpen) close();

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

		std::string camFirmwareDisc = getPropString(vrSys, vr::Prop_CameraFirmwareDescription_String);
		ofLogNotice(__FUNCTION__) << "Camera Firmware: " << camFirmwareDisc;

		bOpen = true;
		return bOpen;
	}

	bool TrackedCamera::close() {
		trackedCamera = nullptr;
		bOpen = false;
		return true;
	}

	bool TrackedCamera::start() {

		if (!bOpen || !trackedCamera) {
			ofLogError(__FUNCTION__) << "Tracked Camera is not opened yet.";
		}

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
			rawFrameBuffer.clear();
			rawFrameBuffer.reserve(bufferSize);
			rawFrameBuffer.assign(bufferSize, 0);
		}
		
		trackedCamera->AcquireVideoStreamingService(vr::k_unTrackedDeviceIndex_Hmd, &trackedCameraHandle);
		if (trackedCameraHandle == INVALID_TRACKED_CAMERA_HANDLE) {
			ofLogError(__FUNCTION__) << "AcquireVideoStreamingService() Failed!";
			return false;
		}

		tex[vr::Eye_Left].allocate(frameWidth, frameHeight / 2, GL_RGBA);
		tex[vr::Eye_Right].allocate(frameWidth, frameHeight / 2, GL_RGBA);

		lastFrameCount = 0;
		bStreaming = true;

		return bStreaming;

	}

	bool TrackedCamera::stop() {
		trackedCamera->ReleaseVideoStreamingService(trackedCameraHandle);
		trackedCameraHandle = INVALID_TRACKED_CAMERA_HANDLE;

		tex[vr::Eye_Left].clear();
		tex[vr::Eye_Right].clear();

		bStreaming = false;

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

		nCameraError = trackedCamera->GetVideoStreamFrameBuffer(trackedCameraHandle, vr::VRTrackedCameraFrameType_Undistorted, rawFrameBuffer.data(), bufferSize, &frameHeader, sizeof(frameHeader));
		if (nCameraError != vr::VRTrackedCameraError_None) {
			return;
		}

		lastFrameCount = frameHeader.nFrameSequence;
		
		// Load raw pixels to eye textures 
		for (int i = 0; i < 2; i++) {
			if (!pix[i].isAllocated()) {
				pix[i].allocate(frameWidth, frameHeight / 2, 4);
			}

			std::vector<uint8_t>::const_iterator first, last;
			if (i == vr::Hmd_Eye::Eye_Left) {
				first = rawFrameBuffer.begin() + bufferSize / 2 + 1;
				last = rawFrameBuffer.end();
			} else {
				first = rawFrameBuffer.begin();
				last = rawFrameBuffer.begin() + bufferSize / 2;
			}
			std::vector<uint8_t> buf(first, last);
			pix[i].setFromPixels(buf.data(), frameWidth, frameHeight / 2, 4);
			tex[i].loadData(pix[i]);
		}

	}


}