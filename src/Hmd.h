#pragma once
#include "openvr.h"
#include "ofVectorMath.h"
#include "ofCamera.h"
#include "ofVboMesh.h"
#include <array>

namespace ofxOpenVrUtil {

	class Hmd {
	public:
		Hmd();
		~Hmd();

		void setup(vr::IVRSystem* vrSys);

		const glm::mat4& getViewMatrix(vr::EVREye e) const {
			return glm::inverse(pose * eye[e].transform);
		}
		const glm::mat4& getProjectionMatrix(vr::EVREye e) const {
			return eye[e].proj;
		}
		const glm::mat4& getViewProjectionMatrix(vr::EVREye e) const {
			return getProjectionMatrix(e) * getViewMatrix(e);
		}
		glm::vec3 getPosition() const { return glm::vec3(pose[3]); }

		uint32_t getEyeWidth() const { return eyeWidth; }
		uint32_t getEyeHeight() const { return eyeHeight; }

		const glm::mat4& getTransformMatrix() const { return pose; }
		const ofVboMesh& getHiddenMesh(vr::Hmd_Eye eye) { return hiddenMesh[eye]; }

	private:
		friend class Interface;

		void loadHiddenAreaMesh(vr::Hmd_Eye eye);
		vr::IVRSystem* vrSys;

		struct Eye {
			glm::mat4 proj;
			glm::mat4 transform;
		};

		std::array<Eye, 2> eye;
		uint32_t eyeWidth, eyeHeight;
		ofParameter<float> farClip, nearClip;
		std::array<ofVboMesh, 2> hiddenMesh;

		glm::mat4 pose;

	};
}