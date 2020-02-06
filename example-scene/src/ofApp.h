#pragma once

//Example of rendering simplest primitives in VR:
//points, lines, triangle
//NOTE: here is rendering without shader.
//For example with shader - see example-drawing.

#include "ofMain.h"
#include "ofxOpenVR.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void exit();

		void update();
		void draw();

		void render(vr::Hmd_Eye nEye);
		void controllerEvent(ofxOpenVRControllerEvent& args);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofxOpenVR openVR;

		
};
