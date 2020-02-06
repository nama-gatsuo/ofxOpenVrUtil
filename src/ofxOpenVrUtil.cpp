#include "ofxOpenVrUtil.h"

namespace ofxOpenVrUtil {
	Interface::Interface() :
		vrSys(nullptr), systemName("No Driver"), modelNumber("No Display"),
		trackedCam(vrSys)
	{}

	void Interface::setup() {

		// Init VR
		vr::EVRInitError e = vr::VRInitError_None;
		vrSys = vr::VR_Init(&e, vr::VRApplication_Scene);
		if (e != vr::VRInitError_None) {
			ofLogError(__FUNCTION__) << vr::VR_GetVRInitErrorAsEnglishDescription(e);
			return;
		}

		// Get Property strings
		systemName = getPropString(vr::Prop_TrackingSystemName_String);
		modelNumber = getPropString(vr::Prop_ModelNumber_String);

	}

	std::string Interface::getPropString(vr::TrackedDeviceProperty prop) {
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

}
