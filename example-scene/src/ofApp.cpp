#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(false);
	vr.setup();

	int eyeWidth = vr.getHmd().getEyeWidth();
	int eyeHeight = vr.getHmd().getEyeHeight();

	for (auto& f : eyeFbo) {
		f.allocate(eyeWidth, eyeHeight, GL_RGBA);
	}

}

void ofApp::update(){
	vr.update();
	
	auto drawCall = [&]() {
		ofNoFill();
		ofDrawBox(glm::vec3(0), 1.f);
		ofDrawAxis(3.);
	};

	for (int i = 0; i < 2; i++) {
		eyeFbo[i].begin();
		vr.beginEye(static_cast<vr::Hmd_Eye>(i));
		drawCall();
		vr.endEye();
		eyeFbo[i].end();

		vr.submit(eyeFbo[i].getTexture(), static_cast<vr::Hmd_Eye>(i));
	}

}

void ofApp::draw(){
	
	
}

void ofApp::exit() {
	vr.exit();
}
