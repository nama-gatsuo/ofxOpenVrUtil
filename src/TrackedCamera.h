#pragma once

#include <array>

#include "openvr.h"
#include "ofLog.h"
#include "ofVectorMath.h"
#include "ofTexture.h"
#include "ofPixels.h"
#include "ofVboMesh.h"
#include "ofShader.h"

namespace ofxOpenVrUtil {
	class TrackedCamera {
	public:
		TrackedCamera();
		~TrackedCamera();

		void setup(vr::IVRSystem* vrSys);

		bool start();
		bool stop();
		bool isStreaming() const { return bStreaming; }

		void update();


		void draw(vr::Hmd_Eye eye);

		uint32_t getTexId() { return texId; }

	private:
		vr::IVRSystem* vrSys;
		vr::IVRTrackedCamera* trackedCamera;
		vr::TrackedCameraHandle_t trackedCameraHandle;
		
		uint32_t frameWidth, frameHeight;
		uint32_t bufferSize;
		
		uint32_t texId;
		ofShader bindTex;

		uint32_t lastFrameCount;
		std::array<ofVboMesh, 2> rect;

		bool bStreaming;
	};
}
