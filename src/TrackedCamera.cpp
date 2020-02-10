#include "TrackedCamera.h"
#include "Utils.h"

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
		}

		bool bHasCamera = false;
		vr::EVRTrackedCameraError e = trackedCamera->HasCamera(vr::k_unTrackedDeviceIndex_Hmd, &bHasCamera);

		if (e != vr::VRTrackedCameraError_None || !bHasCamera) {
			ofLogError(__FUNCTION__) << "No Tracked Camera Available! ( " << trackedCamera->GetCameraErrorNameFromEnum(e) << " )";
		}

		std::string camFirmwareDisc = getPropString(vrSys, vr::Prop_CameraFirmwareDescription_String);
		ofLogNotice(__FUNCTION__) << "Camera Firmware: " << camFirmwareDisc;

		uint32_t size = 0;
		// Allocate for camera frame buffer requirements
		e = trackedCamera->GetCameraFrameSize(
			vr::k_unTrackedDeviceIndex_Hmd,
			vr::VRTrackedCameraFrameType_Undistorted,
			&frameWidth, &frameHeight, &size
		);

		if (e != vr::VRTrackedCameraError_None) {
			ofLogError(__FUNCTION__) << "GetCameraFrameBounds() Failed!";
		}
		ofLogNotice(__FUNCTION__) << "Tracked Cam input size: " << frameWidth << " x " << frameHeight << " (bufferSize: " << size << ")";

		bindTex.load("shader/tex");

		rect[0] = ofMesh::plane(frameWidth, frameHeight / 2, 2, 2);
		rect[1] = ofMesh::plane(frameWidth, frameHeight / 2, 2, 2);

		rect[vr::Eye_Left].clearTexCoords();
		rect[vr::Eye_Left].addTexCoord(glm::vec2(0, 1));
		rect[vr::Eye_Left].addTexCoord(glm::vec2(1, 1));
		rect[vr::Eye_Left].addTexCoord(glm::vec2(0, 0.5));
		rect[vr::Eye_Left].addTexCoord(glm::vec2(1, 0.5));

		rect[vr::Eye_Right].clearTexCoords();
		rect[vr::Eye_Right].addTexCoord(glm::vec2(0, 0.5));
		rect[vr::Eye_Right].addTexCoord(glm::vec2(1, 0.5));
		rect[vr::Eye_Right].addTexCoord(glm::vec2(0, 0));
		rect[vr::Eye_Right].addTexCoord(glm::vec2(1, 0));

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
		vr::EVRTrackedCameraError nCameraError = trackedCamera->GetVideoStreamTextureGL(trackedCameraHandle, vr::VRTrackedCameraFrameType_Undistorted, nullptr, &frameHeader, sizeof(frameHeader));
		if (nCameraError != vr::VRTrackedCameraError_None) return;
		if (frameHeader.nFrameSequence == lastFrameCount) {
			// frame hasn't changed yet, nothing to do
			return;
		}
		//ofLogNotice() << tex[1].getTextureData().textureID;
		nCameraError = trackedCamera->GetVideoStreamTextureGL(trackedCameraHandle, vr::VRTrackedCameraFrameType_Undistorted, &texId, &frameHeader, sizeof(frameHeader));

		//nCameraError = trackedCamera->GetVideoStreamFrameBuffer(trackedCameraHandle, vr::VRTrackedCameraFrameType_Undistorted, rawFrameBuffer.data(), bufferSize, &frameHeader, sizeof(frameHeader));
		if (nCameraError != vr::VRTrackedCameraError_None) {
			ofLogError(__FUNCTION__) << nCameraError;
			return;
		}

		lastFrameCount = frameHeader.nFrameSequence;
		
		// Load raw pixels to eye textures 
		//for (int i = 0; i < 2; i++) {
		//	if (i == vr::Eye_Left) {
		//		tex[i].loadData(&(rawFrameBuffer.data()[bufferSize / 2]), frameWidth, frameHeight / 2, GL_RGBA);
		//	} else {
		//		tex[i].loadData(&(rawFrameBuffer.data()[0]), frameWidth, frameHeight / 2, GL_RGBA);
		//	}
		//	//tex[i].loadData(pix[i]);			
		//}

	}

	void TrackedCamera::draw(vr::Hmd_Eye eye) {
		bindTex.begin();
		glActiveTexture(GL_TEXTURE0 + 0);
		glClientActiveTexture(GL_TEXTURE0 + 0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texId);

		bindTex.setUniform1i("tex", 0);

		rect[eye].draw();

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(0, 0);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);

		bindTex.end();
	}

}