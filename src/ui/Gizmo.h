#pragma once
#include "vrEvents.h"
#include "ofLog.h"
#include "ofParameter.h"
#include "of3dGraphics.h"
#include "ofGraphics.h"
#include "ofShader.h"
#include "ofVboMesh.h"

#include "GizmoComponent.h"
#include "glm/gtx/euler_angles.hpp"

namespace ofxOpenVrUtil {
	class Gizmo {
	public:
		Gizmo() : selectorIndex(-1), selectedType(-1), bListenerActive(false) {}
		~Gizmo() {}

		void setup();

		void onButtonPressed(ofxOpenVrUtil::Event::ButtonInfo& a) {
			if (!bListenerActive) return;

			if (a.buttonId == vr::EVRButtonId::k_EButton_Grip) {
				glm::vec3 localPos = glm::vec3(a.transform[3]) - pos.get();

				int type = 0;
				for (auto& c : components) {
					if (c->checkInside(localPos)) {

						selectedOffset = pos.get() - a.transform[3];
						selectedPos = glm::vec3(a.transform[3]);
						selectedRot = rot.get();

						selectedComponent = c;
						selectedComponent->setSelected(true);
						selectorIndex = a.index;
						selectedType = type;

						break;
					}
					type++;
				}
			}
			
		}

		void onButtonUnpressed(ofxOpenVrUtil::Event::ButtonInfo& a) {
			if (!bListenerActive) return;

			if (a.buttonId == vr::EVRButtonId::k_EButton_Grip) {
				if (selectedComponent) {

					selectedOffset = glm::vec3(0);
					selectedPos = glm::vec3(0);
					selectedRot = glm::vec3(0);

					selectedComponent->setSelected(false);
					selectedComponent = nullptr;
					selectorIndex = -1;
					selectedType = -1;
				}
			}

			
		}


		void onTriggerPressed(ofxOpenVrUtil::Event::AxisMove& a) {
			if (!bListenerActive) return;

			glm::vec3 localPos = glm::vec3(a.transform[3]) - pos.get();

			int type = 0;
			for (auto& c : components) {
				if (c->checkInside(localPos)) {

					selectedOffset = pos.get() - a.transform[3];
					selectedPos = glm::vec3(a.transform[3]);
					selectedRot = rot.get();

					selectedComponent = c;
					selectedComponent->setSelected(true);
					selectorIndex = a.index;
					selectedType = type;

					break;
				}
				type++;
			}
		}

		void onTriggerUnpressed(ofxOpenVrUtil::Event::AxisMove& a) {
			if (!bListenerActive) return;

			if (selectedComponent) {

				selectedOffset = glm::vec3(0);
				selectedPos = glm::vec3(0);
				selectedRot = glm::vec3(0);

				selectedComponent->setSelected(false);
				selectedComponent = nullptr;
				selectorIndex = -1;
				selectedType = -1;
			}
		}

		void onTriggerUpdated(ofxOpenVrUtil::Event::TrackedDeviceMove& a) {
			if (!bListenerActive) return;

			if (selectedComponent && a.index == selectorIndex) {

				glm::vec3 devicePos = glm::vec3(a.transform[3]);
				glm::vec3 delta = devicePos - selectedPos;
				glm::vec3 newP = devicePos + selectedOffset;

				if (selectedType == SPHERE) {
					pos = newP;
				} else if (selectedType == AXIS_X) {
					pos.set(glm::vec3(newP.x, pos->y, pos->z));
				} else if (selectedType == AXIS_Y) {
					pos.set(glm::vec3(pos->x, newP.y, pos->z));
				} else if (selectedType == AXIS_Z) {
					pos.set(glm::vec3(pos->x, pos->y, newP.z));
				} else if (selectedType == ROT_X) {
					glm::vec2 d(delta.y, delta.z);
					float t = (d.x + d.y) * 4.0f;
					rot.set(glm::vec3(selectedRot.x + t, rot->y, rot->z));
				} else if (selectedType == ROT_Y) {
					glm::vec2 d(delta.x, delta.z);
					float t = (d.x + d.y) * 4.0f;
					rot.set(glm::vec3(rot->x, selectedRot.y + t, rot->z));
				} else if (selectedType == ROT_Z) {
					glm::vec2 d(delta.y, delta.x);
					float t = (d.x + d.y) * 4.0f;
					rot.set(glm::vec3(rot->x, rot->y, selectedRot.z + t));
				}

			}
		}

		void draw();

		void enableListener() { bListenerActive = true; }
		void disableListener() { bListenerActive = false; }

		void pushMatrix(bool isDraw) {
			ofPushMatrix();
			ofMultMatrix(glm::translate(pos.get()));

			if (isDraw) draw();

			ofMultMatrix(glm::eulerAngleXYZ(rot->x, rot->y, rot->z));
		}

		void popMatrix() {
			ofPopMatrix();
		}

		glm::mat4 getTransform() const {
			return glm::translate(pos.get()) * glm::eulerAngleXYZ(rot->x, rot->y, rot->z);
		}

		ofParameterGroup& getParameters() { return group; }

	private:

		enum ComponentID { SPHERE = 0, AXIS_X, AXIS_Y, AXIS_Z, ROT_X, ROT_Y, ROT_Z };

		void createComponents();

		ofParameterGroup group;
		ofParameter<glm::vec3> pos;
		ofParameter<glm::vec3> rot;
		ofParameter<float> radius;

		bool bListenerActive;

		int selectorIndex;
		glm::vec3 selectedPos;
		glm::vec3 selectedOffset;
		glm::vec3 selectedRot;
		int selectedType;
		ofPtr<GizmoComponent> selectedComponent;

		std::vector<ofPtr<GizmoComponent>> components;

	};
}

