#pragma once

#include <array>

#include "openvr.h"
#include "ofLog.h"
#include "ofVectorMath.h"
#include "ofTexture.h"
#include "ofPixels.h"

namespace ofxOpenVrUtil {
	class TrackedCamera {
	public:
		TrackedCamera(vr::IVRSystem* vrSys);
		~TrackedCamera();

		bool open();
		bool close();

		bool start();
		bool stop();

		void update();

		const ofTexture getTexture(vr::Hmd_Eye eye) const {
			return tex[eye];
		}

	private:
		vr::IVRSystem* vrSys;
		vr::IVRTrackedCamera* trackedCamera;
		vr::TrackedCameraHandle_t trackedCameraHandle;
		
		uint32_t frameWidth, frameHeight;
		uint32_t bufferSize;
		uint8_t* rawFrameBuffer;
		std::array<ofTexture, 2> tex;
		std::array<ofPixels, 2> pix;
		uint32_t lastFrameCount;
	};
}
