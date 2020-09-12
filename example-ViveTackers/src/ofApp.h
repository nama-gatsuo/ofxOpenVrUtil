#pragma once

#include "ofMain.h"
#include "ofxOpenVrUtil.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void exit();
	
private:
	ofxOpenVrUtil::NoHmdInterface openVR;
	ofEasyCam cam;
};
