#include "TrackedCamera.h"
#include "Utils.h"

namespace ofxOpenVrUtil {

	TrackedCamera::TrackedCamera() : vrSys(nullptr), trackedCamera(nullptr), bOpen(false), bStreaming(false) {}
	TrackedCamera::~TrackedCamera() {
		if (bStreaming) stop();
		if (bOpen) close();

		vrSys = nullptr;
		trackedCamera = nullptr;
	}

	void TrackedCamera::setup(vr::IVRSystem* vrSys) {
		this->vrSys = vrSys;
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
			ofLogError(__FUNCTION__) << "No Tracked Camera Available! ( " << trackedCamera->GetCameraErrorNameFromEnum(e) << " )";
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
			return false;
		}

		uint32_t size = 0;
		// Allocate for camera frame buffer requirements
		vr::EVRTrackedCameraError e = trackedCamera->GetCameraFrameSize(
			vr::k_unTrackedDeviceIndex_Hmd,
			vr::VRTrackedCameraFrameType_Undistorted,
			&frameWidth, &frameHeight, &size
		);

		if (e != vr::VRTrackedCameraError_None) {
			ofLogError(__FUNCTION__) << "GetCameraFrameBounds() Failed!";
			return false;
		}
		ofLogNotice(__FUNCTION__) << "Tracked Cam input size: " << frameWidth << " x " << frameHeight << " (bufferSize: " << size << ")";

		if (size != 0 && size != bufferSize) {
			bufferSize = size;
			rawFrameBuffer.clear();
			rawFrameBuffer.reserve(bufferSize);
			rawFrameBuffer.assign(bufferSize, 0);
		}
		
		trackedCamera->AcquireVideoStreamingService(vr::k_unTrackedDeviceIndex_Hmd, &trackedCameraHandle);
		if (trackedCameraHandle == INVALID_TRACKED_CAMERA_HANDLE) {
			ofLogError(__FUNCTION__) << "AcquireVideoStreamingService() Failed!";
			return false;
		}

		pix[vr::Eye_Left].allocate(frameWidth, frameHeight / 2, 4);
		tex[vr::Eye_Left].allocate(frameWidth, frameHeight / 2, GL_RGBA8);

		pix[vr::Eye_Right].allocate(frameWidth, frameHeight / 2, 4);
		tex[vr::Eye_Right].allocate(frameWidth, frameHeight / 2, GL_RGBA8);

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
			if (i == vr::Eye_Left) {
				tex[i].loadData(&(rawFrameBuffer.data()[bufferSize / 2]), frameWidth, frameHeight / 2, GL_RGBA);
			} else {
				tex[i].loadData(&(rawFrameBuffer.data()[0]), frameWidth, frameHeight / 2, GL_RGBA);
			}
			//tex[i].loadData(pix[i]);			
		}

	}

}