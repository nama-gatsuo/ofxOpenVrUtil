#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(false);
	vr.setup();
	vr.getTrackedCamera().start();

	int eyeWidth = vr.getHmd().getEyeWidth();
	int eyeHeight = vr.getHmd().getEyeHeight();

	// Let's prepare fbos to be renderred for each eye
	ofFboSettings s;
	s.width = eyeWidth;   // Actual rendering resolution is higer than actual display(HMD) has,
	s.height = eyeHeight; // because VR system will distort images like barrel distortion and fit it to display through lens
	s.maxFilter = GL_NEAREST;
	s.minFilter = GL_NEAREST;
	s.numSamples = 4; // MSAA enabled. Anti-Alising is much important for VR experience
	s.textureTarget = GL_TEXTURE_2D; // Can't use GL_TEXTURE_RECTANGLE_ARB which is default in oF
	s.internalformat = GL_RGBA8;
	s.useDepth = true;
	s.useStencil = true;

	for (auto& f : eyeFbo) {
		f.allocate(s);
	}

	panel.setup();
	panel.add(eyeScale.set("eyeScale", 3.f, 2.f, 4.f));
	panel.add(eyeOffset.set("eyeOffset", glm::vec2(10), glm::vec2(-500), glm::vec2(500)));
	

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
		vr.applyEyeStencil(static_cast<vr::Hmd_Eye>(i));

		if (vr.getTrackedCamera().isStreaming()) {
		
			
			ofSetRectMode(OF_RECTMODE_CENTER);
			ofPushMatrix();
			ofTranslate(eyeFbo[i].getWidth() / 2, eyeFbo[i].getHeight() / 2);
			ofTranslate(pow(-1, i) * eyeOffset->x, eyeOffset->y);
			ofScale(eyeScale);

			vr.getTrackedCamera().draw(static_cast<vr::Hmd_Eye>(i));
			//ofMesh::plane(t.getWidth(), t.getHeight(), 2, 2).draw();

			//t.draw(0, 0);
			ofPopMatrix();
			ofSetRectMode(OF_RECTMODE_CORNER);
			
		}
		
		vr.beginEye(static_cast<vr::Hmd_Eye>(i));
		drawCall();

		for (auto& c : vr.getControllers()) {
			c.second->draw();
		}
		
		vr.endEye();

		eyeFbo[i].end();

		vr.submit(eyeFbo[i].getTexture(), static_cast<vr::EVREye>(i));
	}

}

void ofApp::draw(){
	int w = vr.getHmd().getEyeWidth();
	int h = vr.getHmd().getEyeHeight();

	
	ofPushMatrix();
	ofScale(1.f / 4.f);
	eyeFbo[vr::Eye_Left].draw(0, 0, w, h);
	eyeFbo[vr::Eye_Right].draw(w, 0, w, h);
	ofPopMatrix();

	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 12, 16);
	
	panel.draw();
}

void ofApp::exit() {
	vr.getTrackedCamera().stop();
	vr.exit();
}
