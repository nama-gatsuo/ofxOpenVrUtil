#include "Gizmo.h"

namespace ofxOpenVrUtil {
	void Gizmo::setup() {
		ofAddListener(ofxOpenVrUtil::Event::onTriggerPressed, this, &Gizmo::onTriggerPressed);
		ofAddListener(ofxOpenVrUtil::Event::onTriggerUnpressed, this, &Gizmo::onTriggerUnpressed);
		ofAddListener(ofxOpenVrUtil::Event::onButtonPressed, this, &Gizmo::onButtonPressed);
		ofAddListener(ofxOpenVrUtil::Event::onButtonUnpressed, this, &Gizmo::onButtonUnpressed);
		ofAddListener(ofxOpenVrUtil::Event::onControllerMove, this, &Gizmo::onTriggerUpdated);

		group.setName("Gizmo");
		group.add(pos.set("position", glm::vec3(0, 1.0f, 0), glm::vec3(-10.f), glm::vec3(10.f)));
		group.add(rot.set("rotation", glm::vec3(0), glm::vec3(-180.f), glm::vec3(180.f)));

		createComponents();
	}

	void Gizmo::createComponents() {

		components.resize(7);

		ofPtr<GizmoSphere> sphere = std::make_shared<GizmoSphere>();
		sphere->setup();
		components[SPHERE] = sphere;

		ofPtr<GizmoArrow> arrowX = std::make_shared<GizmoArrow>();
		arrowX->setup();
		arrowX->setTransform(glm::rotate(float(HALF_PI), glm::vec3(0, 0, 1)));
		arrowX->setColor(ofFloatColor(1, 0.3f, 0.3f));
		components[AXIS_X] = arrowX;

		ofPtr<GizmoArrow> arrowY = std::make_shared<GizmoArrow>();
		arrowY->setup();
		arrowY->setColor(ofFloatColor(0.3f, 1, 0.3f));
		components[AXIS_Y] = arrowY;

		ofPtr<GizmoArrow> arrowZ = std::make_shared<GizmoArrow>();
		arrowZ->setup();
		arrowZ->setTransform(glm::rotate(float(HALF_PI), glm::vec3(1, 0, 0)));
		arrowZ->setColor(ofFloatColor(0.3f, 0.3f, 1));
		components[AXIS_Z] = arrowZ;

		ofPtr<GizmoArc> arcX = std::make_shared<GizmoArc>();
		arcX->setup();
		arcX->setTransform(glm::rotate(float(-HALF_PI), glm::vec3(0, 0, 1)));
		arcX->setColor(ofFloatColor(1, 0.3f, 0.3f));
		components[ROT_X] = arcX;

		ofPtr<GizmoArc> arcY = std::make_shared<GizmoArc>();
		arcY->setup();
		arcY->setColor(ofFloatColor(0.3f, 1, 0.3f));
		components[ROT_Y] = arcY;

		ofPtr<GizmoArc> arcZ = std::make_shared<GizmoArc>();
		arcZ->setup();
		arcZ->setTransform(glm::rotate(float(-HALF_PI), glm::vec3(1, 0, 0)));
		arcZ->setColor(ofFloatColor(0.3f, 0.3f, 1.f));
		components[ROT_Z] = arcZ;

	}

	void Gizmo::draw() {

		ofPushMatrix();
		if (selectedComponent && selectedType == ROT_X) {
			ofSetColor(255, 255, 0);
			ofNoFill();
			ofPushMatrix();
			ofRotateY(90);
			ofDrawCircle(glm::vec3(0), 0.2f);
			ofDrawCircle(glm::vec3(0), 0.16f);
			ofPopMatrix();
			ofFill();

			ofDrawLine(glm::vec3(0, 0, 0.2f), glm::vec3(0, 0, 0.16f));
			ofRotateX(ofRadToDeg(rot->x - selectedRot.x));
			ofDrawLine(glm::vec3(0, 0, 0.2f), glm::vec3(0, 0, 0.16f));
			ofSetColor(255);
		} else if (selectedComponent && selectedType == ROT_Y) {
			ofSetColor(255, 255, 0);
			ofNoFill();
			ofPushMatrix();
			ofRotateX(90);
			ofDrawCircle(glm::vec3(0), 0.2f);
			ofDrawCircle(glm::vec3(0), 0.16f);
			ofPopMatrix();
			ofFill();

			ofDrawLine(glm::vec3(0.2f, 0, 0), glm::vec3(0.16f, 0, 0));
			ofRotateY(ofRadToDeg(rot->y - selectedRot.y));
			ofDrawLine(glm::vec3(0.2f, 0, 0), glm::vec3(0.16f, 0, 0));
			ofSetColor(255);
		} else if (selectedComponent && selectedType == ROT_Z) {
			ofSetColor(255, 255, 0);
			ofNoFill();
			ofPushMatrix();
			ofDrawCircle(glm::vec3(0), 0.2f);
			ofDrawCircle(glm::vec3(0), 0.16f);
			ofPopMatrix();
			ofFill();

			ofDrawLine(glm::vec3(0, 0.2f, 0), glm::vec3(0, 0.16f, 0));
			ofRotateZ(ofRadToDeg(rot->z - selectedRot.z));
			ofDrawLine(glm::vec3(0, 0.2f, 0), glm::vec3(0, 0.16f, 0));
			ofSetColor(255);
		} else {
			for (auto c : components) {
				c->draw(false);
			}
		}

		ofPopMatrix();

	}
}