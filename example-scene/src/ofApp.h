#pragma once

#include "ofMain.h"
#include "ofxOpenVrUtil.h"
#include "ofxGui.h"
#include "ui/Gizmo.h"
#include "ui/GpuRay.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
		
	void exit();
	void keyPressed(int key);
		
private:
	void drawGrid();
	void drawObject();

	ofxOpenVrUtil::Interface vr;
	std::array<ofFbo, 2> eyeFbo;

	ofxOpenVrUtil::Gizmo gizmo;
	ofxOpenVrUtil::GpuRay ray;
	ofShader lambert;
	ofVboMesh box;
};
