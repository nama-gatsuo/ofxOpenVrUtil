#pragma once

#include "ofMain.h"
#include "ofxOpenVrUtil.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		
		void exit();

		
private:
	ofxOpenVrUtil::Interface vr;
	std::array<ofFbo, 2> eyeFbo;

	ofxPanel panel;
	ofParameter<float> eyeScale;
	ofParameter<glm::vec2> eyeOffset;

	ofShader bindTex;
};
