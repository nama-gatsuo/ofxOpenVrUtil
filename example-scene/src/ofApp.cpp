#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(false);
	vr.setup();
	vr.getTrackedCamera().open();
	vr.getTrackedCamera().start();

	int eyeWidth = vr.getHmd().getEyeWidth();
	int eyeHeight = vr.getHmd().getEyeHeight();

	ofDisableArbTex();
	for (auto& f : eyeFbo) {
		f.allocate(eyeWidth, eyeHeight, GL_RGBA);
	}
	ofEnableArbTex();

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
		ofClear(0);

		if (vr.getTrackedCamera().isStreaming()) {
			float camScale = 1.8;
			auto& t = vr.getTrackedCamera().getTexture(static_cast<vr::Hmd_Eye>(i));

			ofPushMatrix();
			ofTranslate(eyeFbo[i].getWidth() / 2, eyeFbo[i].getHeight() / 2);
			ofScale(camScale);
			ofTranslate(t.getWidth() / 2, -t.getHeight() / 2);
			t.draw(0, 0);
			ofPopMatrix();
		}
		
		vr.beginEye(static_cast<vr::Hmd_Eye>(i));
		drawCall();
		vr.endEye();
		eyeFbo[i].end();

		vr.submit(eyeFbo[i].getTexture(), static_cast<vr::EVREye>(i));
	}

}

void ofApp::draw(){
	int w = vr.getHmd().getEyeWidth();
	int h = vr.getHmd().getEyeHeight();

	eyeFbo[vr::Eye_Left].draw(0, 0, w / 3, h / 3);
	eyeFbo[vr::Eye_Right].draw(w / 3, 0, w / 3, h / 3);

	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 12, 16);
	
}

void ofApp::exit() {
	vr.getTrackedCamera().stop();
	vr.getTrackedCamera().close();
	vr.exit();
}
