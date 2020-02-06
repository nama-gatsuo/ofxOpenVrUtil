#include "ofApp.h"

#define STRINGIFY(A) #A

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(false);

	// We need to pass the method we want ofxOpenVR to call when rending the scene
	openVR.setup(std::bind(&ofApp::render, this, std::placeholders::_1));
	openVR.setDrawControllers(true);
}

//--------------------------------------------------------------
void ofApp::exit(){
	openVR.exit();
}

//--------------------------------------------------------------
void ofApp::update(){
	openVR.update();
	while (openVR.hasControllerEvents()) {
		ofxOpenVRControllerEvent event;
		openVR.getNextControllerEvent(event);
		controllerEvent(event);
	}

	
	openVR.render();
}

//--------------------------------------------------------------
void ofApp::draw(){
	//openVR.renderDistortion();
	openVR.renderScene(vr::Eye_Left);
	//openVR.draw_using_contrast_shader(ofGetWidth(), ofGetHeight());

	//openVR.drawDebugInfo(10.0f, 500.0f);
	
}

//--------------------------------------------------------------
void draw_points(float x, float y, float z, float scl) {
	ofPushMatrix();
	ofTranslate(x, y, z);
	ofMesh mesh;
	for (int z = 0; z <= 1; z++) {
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				mesh.addVertex(ofPoint(x*scl, y*scl, z*scl));
			}
		}
	}


	ofSetColor(255);
	mesh.drawVertices();

	ofPopMatrix();
}

//--------------------------------------------------------------
void draw_lines(float x, float y, float z, float scl) {
	ofPushMatrix();
	ofTranslate(x, y, z);

	ofSetColor(255);
	ofLine(-scl, 0, -scl, scl, 0, -scl);
	ofLine(-scl, -scl, -scl, -scl, scl, -scl);
	ofLine(-scl, 0, -scl, -scl, 0, scl);

	ofPopMatrix();
}

//--------------------------------------------------------------
void draw_triangles(float x, float y, float z, float scl) {
	ofPushMatrix();
	ofTranslate(x, y, z);

	ofSetColor(255);
	ofFill();
	ofDrawTriangle(scl, 0, 0, 0, scl, 0, 0, 0, scl);

	ofPopMatrix();
}

//--------------------------------------------------------------
void  ofApp::render(vr::Hmd_Eye nEye){
	//Prepare matrices for VR rendering
	//Note, this function also calls openVR.flipVr(), 
	//so some rendering may be flipped.
	//For 2D rendering in FBO you can call openVR.flipOf(), 
	//and then restore the mode back by calling openVR.flipVr().
	openVR.pushMatricesForRender(nEye);

	ofBackground(0);

	draw_points(-1, 1.4, -0.5, 0.2);

	draw_lines(0, 1.4, -1, 0.2);

	draw_triangles(1, 1.4, 0, 0.2);


	openVR.popMatricesForRender();
}

//--------------------------------------------------------------
void ofApp::controllerEvent(ofxOpenVRControllerEvent& args){
	cout << "ofApp::controllerEvent > role: " << ofToString(int(args.controllerRole)) 
		<< " - event type: " << ofToString(int(args.eventType)) 
		<< " - button type: " << ofToString(int(args.buttonType))
		<< " - x: " << args.analogInput_xAxis 
		<< " - y: " << args.analogInput_yAxis << endl;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
