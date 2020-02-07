#include "Hmd.h"
#include "ofLog.h"
#include "Utils.h"

namespace ofxOpenVrUtil {
	Hmd::Hmd(vr::IVRSystem* vrSys) : vrSys(vrSys) {
	}
	Hmd::~Hmd() {
		vrSys = nullptr;
	}
	void Hmd::setup(vr::IVRSystem* vrSys) {
		if (!vrSys) ofLogError(__FUNCTION__) << "IVRSystem is not initialized.";
		
		for (int i = 0; i < 2; i++) {

			eye[i].proj = toGlm(vrSys->GetProjectionMatrix(static_cast<vr::EVREye>(i), nearClip.get(), farClip.get()));
			eye[i].transform = toGlm(vrSys->GetEyeToHeadTransform(static_cast<vr::EVREye>(i)));

		}

		vrSys->GetRecommendedRenderTargetSize(&eyeWidth, &eyeHeight);
		ofLogNotice(__FUNCTION__) << "Recommeded Render Size (per eye): " << eyeWidth << " x " << eyeHeight;

 	}
}