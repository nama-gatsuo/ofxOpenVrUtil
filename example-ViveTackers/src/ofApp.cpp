#include "ofApp.h"

void ofApp::setup(){

	vr::EVRInitError e = vr::VRInitError_None;
	// Initialize without activating VR
	sys = vr::VR_Init(&e, vr::VRApplication_Background);
	if (e != vr::VRInitError_None) {
		ofLogError(__FUNCTION__) << vr::VR_GetVRInitErrorAsEnglishDescription(e);
	}
	trackedDevivePose.resize(vr::k_unMaxTrackedDeviceCount);
}

void ofApp::update(){
	
	if (sys) {
	
		sys->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseRawAndUncalibrated, 0, trackedDevivePose.data(), vr::k_unMaxTrackedDeviceCount);
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			if (trackedDevivePose[i].bPoseIsValid) {
				vr::TrackedDeviceClass deviceClass = sys->GetTrackedDeviceClass(i);
				std::string n;
				if (deviceClass == vr::TrackedDeviceClass_GenericTracker) n = "tracker";
				else if (deviceClass == vr::TrackedDeviceClass_TrackingReference) n = "ref";
				else n = "unknown";

				glm::mat4 m(ofxOpenVrUtil::toGlm(trackedDevivePose[i].mDeviceToAbsoluteTracking));
				glm::vec3 r = glm::eulerAngles(glm::toQuat(m));

				glm::vec3 p(m[3]);

				ofLogNotice(n + " p") << p;
				ofLogNotice(n + " r") << r;
			}
		}
	
	}


}

void ofApp::draw(){

}

void ofApp::exit() {
	if (sys) {
		vr::VR_Shutdown();
		sys = nullptr;
	}
}
