#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(false);
	ofSetLogLevel(OF_LOG_VERBOSE);
	vr.setup();

	int eyeWidth = vr.getHmd().getEyeWidth();
	int eyeHeight = vr.getHmd().getEyeHeight();

	// Let's prepare fbos to be renderred for each eye
	ofFboSettings s;
	s.width = eyeWidth;   // Actual rendering resolution is higer than display(HMD) spec has,
	s.height = eyeHeight; // because VR system will distort images like barrel distortion and fit them through lens
	s.maxFilter = GL_LINEAR;
	s.minFilter = GL_LINEAR;
	s.numSamples = 4; // MSAA enabled. Anti-Alising is much important for VR experience
	s.textureTarget = GL_TEXTURE_2D; // Can't use GL_TEXTURE_RECTANGLE_ARB which is default in oF
	s.internalformat = GL_RGBA8;
	s.useDepth = true;
	s.useStencil = true;

	for (auto& f : eyeFbo) {
		f.allocate(s);
	}

	gizmo.setup();
	gizmo.enableListener();

	ray.setup();

	lambert.load("shader/lambert");
	box = ofMesh::box(0.3, 0.3, 0.3, 1, 1, 1);
}

void ofApp::update(){
	vr.update();

	auto drawCall = [&]() {

		drawGrid();
		

		gizmo.pushMatrix(true);
		lambert.begin();
		lambert.setUniform3f("lightDir", glm::vec3(1));
		lambert.setUniform4f("baseColor", ofFloatColor(0.7f));
		lambert.setUniformMatrix4f("viewMatrix", glm::inverse(vr.getHmd().getTransformMatrix()));
		drawObject();
		lambert.end();
		gizmo.popMatrix();

	};

	for (int i = 0; i < 2; i++) {
		
		eyeFbo[i].begin();
		ofClear(0);
		vr.applyEyeStencil(static_cast<vr::Hmd_Eye>(i));
		vr.beginEye(static_cast<vr::Hmd_Eye>(i));
		{
		
			if (vr.getTrackedCamera().isStreaming()) {
				vr.getTrackedCamera().draw(static_cast<vr::Hmd_Eye>(i));
			}

			drawCall();
			
			for (auto& c : vr.getControllers()) {
				c.second->draw();
				ofDrawSphere(c.second->getPosition(), 0.01f);
			}

		}		
		vr.endEye();
		eyeFbo[i].end();

		// Submit texture to VR!
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
	
}

void ofApp::exit() {
	if (vr.getTrackedCamera().isStreaming()) vr.getTrackedCamera().stop();
	vr.exit();
}

void ofApp::keyPressed(int key) {
	if (key == ' ') {
		if (vr.getTrackedCamera().isStreaming()) {
			vr.getTrackedCamera().stop();
		} else {
			vr.getTrackedCamera().start();
		}
	}
}

void ofApp::drawGrid() {
	ofPushStyle();
	{
		ofSetColor(100);
		for (float x = -1.5f; x <= 1.5f; x += 1.f) {
			ofDrawLine(x, 0, -2.f, x, 0, 2.f);
		}
		for (float y = -1.5f; y <= 1.5f; y += 1.f) {
			ofDrawLine(-2.f, 0, y, 2.f, 0, y);
		}
	}
	{
		ofSetColor(196);
		for (float x = -2.f; x <= 2.f; x += 1.f) {
			if (x != 0.f) ofDrawLine(x, 0, -2.f, x, 0, 2.f);
		}
		for (float y = -2.f; y <= 2.f; y += 1.f) {
			if (y != 0.f) ofDrawLine(-2.f, 0, y, 2.f, 0, y);
		}
	}

	ofSetColor(255, 0, 128);
	ofDrawLine(-2.f, 0, 0, 2.f, 0, 0);
	ofDrawLine(0, 0, -2.f, 0, 0, 2.f);

	ofPopStyle();
}

void ofApp::drawObject() {
	ofPushMatrix();
	ofTranslate(0, .5, 0);
	for (int i = 0; i < 24; i++) {
		ofRotateY(1.f + ofGetElapsedTimef() * 1.f);
		ofRotateZ(-1.f + ofGetElapsedTimef() * 2.f);
		box.draw();
	}
	ofPopMatrix();
}

