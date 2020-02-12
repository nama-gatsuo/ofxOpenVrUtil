#pragma once
#include "openvr.h"
#include "ofLog.h"

namespace ofxOpenVrUtil {
	static std::string getPropString(vr::IVRSystem * vrSys, vr::TrackedDeviceProperty prop) {
		if (!vrSys) ofLogError(__FUNCTION__) << "IVRSystem is not initialized.";

		vr::TrackedPropertyError e;
		uint32_t len = vrSys->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, prop, NULL, 0, &e);

		if (len == 0) {
			ofLogError(__FUNCTION__) << "No property found.";
			return "";
		} else {
			
			char* buf = new char[len];
			len = vrSys->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, prop, buf, len, &e);
			std::string result = buf;
			delete[] buf;
			return result;
		}
	}

	static std::string getPropString(vr::IVRSystem* vrSys, vr::TrackedDeviceIndex_t deviceIndex, vr::TrackedDeviceProperty prop) {
		if (!vrSys) ofLogError(__FUNCTION__) << "IVRSystem is not initialized.";

		vr::TrackedPropertyError e;
		uint32_t len = vrSys->GetStringTrackedDeviceProperty(deviceIndex, prop, NULL, 0, &e);

		if (len == 0) {
			ofLogError(__FUNCTION__) << "No property found.";
			return "";
		} else {

			char* buf = new char[len];
			len = vrSys->GetStringTrackedDeviceProperty(deviceIndex, prop, buf, len, &e);
			std::string result = buf;
			delete[] buf;
			return result;
		}
	}

	static glm::mat4 toGlm(const vr::HmdMatrix44_t& m) {
		return glm::mat4x4(
			m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
			m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
			m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
			m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]
		);
	}

	static glm::mat4 toGlm(const vr::HmdMatrix34_t& m) {
		return glm::mat4x4(
			m.m[0][0], m.m[1][0], m.m[2][0], 0.f,
			m.m[0][1], m.m[1][1], m.m[2][1], 0.f,
			m.m[0][2], m.m[1][2], m.m[2][2], 0.f,
			m.m[0][3], m.m[1][3], m.m[2][3], 1.f
		);
	}
	static glm::mat4 toGlmN(const vr::HmdMatrix34_t& m) {
		return glm::mat4x4(
			m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3],
			m.m[1][0], m.m[1][1], m.m[1][2], m.m[1][3],
			m.m[2][0], m.m[2][1], m.m[2][2], m.m[2][3],
			0.f, 0.f, 0.f, 1.f
		);
	}

	static const glm::vec3& toGlm(const vr::HmdVector3_t& v) {
		return *reinterpret_cast<const glm::vec3*>(&v);
	}
	static const glm::vec2& toGlm(const vr::HmdVector2_t& v) {
		return *reinterpret_cast<const glm::vec2*>(&v);
	}
	
	static glm::vec2 toGlm(const float* v) {
		return glm::vec2(v[0], v[1]);
	}

}