#pragma once

//Example of rendering simplest primitives in VR:
//points, lines, triangle
//NOTE: here is rendering without shader.
//For example with shader - see example-drawing.

#include "ofMain.h"
#include "ofxOpenVrUtil.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		
		void exit();

		
private:
	ofxOpenVrUtil::Interface vr;
	std::array<ofFbo, 2> eyeFbo;
};
