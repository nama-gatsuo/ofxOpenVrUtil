#pragma once
#include "openvr.h"
#include "ofVectorMath.h"
#include "ofCamera.h"
#include <array>

namespace ofxOpenVrUtil {

	class Hmd {
	public:
		Hmd(vr::IVRSystem* vrSys);
		~Hmd();

		void setup(vr::IVRSystem* vrSys);

		const glm::mat4& getViewMatrix(vr::Hmd_Eye e) const {
			return glm::inverse(hmdTransform * eye[e].transform);
		}
		const glm::mat4& getProjectionMatrix(vr::Hmd_Eye e) const {
			return eye[e].proj;
		}
		const glm::mat4& getViewProjectionMatrix(vr::Hmd_Eye e) const {
			return getProjectionMatrix(e) * getViewMatrix(e);
		}

		uint32_t getEyeWidth() const { return eyeWidth; }
		uint32_t getEyeHeight() const { return eyeHeight; }

		void setTransformMatrix(const glm::mat4& m) { hmdTransform = m; }
		const glm::mat4& getTransformMatrix() const { return hmdTransform; }

	private:
		vr::IVRSystem* vrSys;

		struct Eye {
			glm::mat4 proj;
			glm::mat4 transform;
		};

		std::array<Eye, 2> eye;
		uint32_t eyeWidth, eyeHeight;
		ofParameter<float> farClip, nearClip;

		glm::mat4 hmdTransform;
		
	};
}