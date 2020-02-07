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
			std::string result;
			char* buf = new char[len];
			len = vrSys->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, prop, buf, len, &e);
			std::string sResult = buf;
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

}