#include "ofApp.h"

void ofApp::setup(){

	openVR.setup();

	cam.disableMouseInput();
	cam.setPosition(300, 200, 300);
	cam.lookAt(glm::vec3(0));
	cam.enableMouseInput();

	ofSetSphereResolution(6);

	ofBackground(0);
	ofNoFill();
}

void ofApp::update(){
	
	openVR.update();

}

void ofApp::draw(){

	cam.begin();
	
	ofScale(100.f);

	ofSetColor(64);
	ofPushMatrix();
	{
		ofRotate(90);
		ofDrawGridPlane(0.5, 10, true);
		ofDrawAxis(0.4);
	}
	ofPopMatrix();

	ofSetColor(255);

	int index = 0;
	for (const auto& tracker : openVR.getTrackers()) {
		glm::mat4 m = tracker->transform;

		ofPushMatrix();
		ofMultMatrix(m);
		ofDrawAxis(0.1f);
		ofDrawSphere(0.06f);

		std::string s = "tracker[" + ofToString(index) + "]\n";
			
		glm::vec3 p(m[3]);
		glm::vec3 r(glm::eulerAngles(glm::toQuat(m)));

		s += "\tpos: [" + ofToString(p.x, 3) + ", " + ofToString(p.y, 3) + ", " + ofToString(p.z, 3) + "]\n";
		s += "\trot: [" + ofToString(r.x, 3) + ", " + ofToString(r.y, 3) + ", " + ofToString(r.z, 3) + "]";

		ofDrawBitmapString(s, glm::vec2(0));

		ofPopMatrix();

		index++;
	}

	index = 0;
	for (const auto& trackerRef : openVR.getTrackerRefs()) {
		glm::mat4 m = trackerRef->transform;

		ofPushMatrix();
		ofMultMatrix(m);
		ofDrawAxis(0.1f);
		ofDrawBox(0.1f);
		ofDrawBitmapString("base[" + ofToString(index) + "]", glm::vec2(0));
		ofPopMatrix();

		index++;
	}

	cam.end();
}

void ofApp::exit() {

	openVR.exit();

}
