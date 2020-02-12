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
	void keyPressed(int key);
		
private:
	void drawGrid();

	ofxOpenVrUtil::Interface vr;
	std::array<ofFbo, 2> eyeFbo;
};
