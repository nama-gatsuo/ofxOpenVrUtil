#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofGLWindowSettings settings;
	settings.setGLVersion(4, 5);
	settings.setSize(1024,720);
	ofCreateWindow(settings);
	ofRunApp(new ofApp());
}
