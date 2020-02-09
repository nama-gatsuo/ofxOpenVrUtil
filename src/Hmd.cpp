#include "Hmd.h"
#include "ofLog.h"
#include "Utils.h"

namespace ofxOpenVrUtil {
	Hmd::Hmd() : vrSys(nullptr) {}
	Hmd::~Hmd() {
		vrSys = nullptr;
	}
	void Hmd::setup(vr::IVRSystem* vrSys) {
		this->vrSys = vrSys;

		if (!vrSys) ofLogError(__FUNCTION__) << "IVRSystem is not initialized.";
		
		nearClip = 0.1f;
		farClip = 30.f;

		for (int i = 0; i < 2; i++) {
			eye[i].proj = toGlm(vrSys->GetProjectionMatrix(static_cast<vr::EVREye>(i), nearClip.get(), farClip.get()));
			eye[i].transform = toGlm(vrSys->GetEyeToHeadTransform(static_cast<vr::EVREye>(i)));

			loadHiddenAreaMesh(static_cast<vr::Hmd_Eye>(i));
		}

		vrSys->GetRecommendedRenderTargetSize(&eyeWidth, &eyeHeight);
		ofLogNotice(__FUNCTION__) << "Recommeded Render Size (per eye): " << eyeWidth << " x " << eyeHeight;

 	}
	void Hmd::loadHiddenAreaMesh(vr::Hmd_Eye eye) {
		if (!vrSys) ofLogError(__FUNCTION__) << "IVRSystem is not initialized.";

		const vr::HiddenAreaMesh_t& am = vrSys->GetHiddenAreaMesh(eye);
		hiddenMesh[eye].setMode(OF_PRIMITIVE_TRIANGLES);
		for (int i = 0; i < am.unTriangleCount; i++) {
			hiddenMesh[eye].addVertex(glm::vec3(toGlm(am.pVertexData[i]), 0));
		}

		if (hiddenMesh[eye].hasVertices()) {
			ofLogNotice(__FUNCTION__) << "Hiedden Area Mesh loaded. Vertex count is " << hiddenMesh[eye].getNumVertices();
		} else {
			ofLogError(__FUNCTION__) << "Can't load hiedden Area Mesh loaded. ";
		}

	}
}