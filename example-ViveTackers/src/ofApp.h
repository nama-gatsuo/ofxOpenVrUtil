#pragma once

#include "ofMain.h"
#include "ofxOpenVrUtil.h"
//#include "openvr_driver.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void exit();
	
private:
	vr::IVRSystem* sys;
	std::vector<vr::TrackedDevicePose_t> trackedDevivePose;
};
