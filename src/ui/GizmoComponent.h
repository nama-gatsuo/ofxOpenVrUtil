#pragma once
#include "vrEvents.h"
#include "ofLog.h"
#include "BoundingShape.h"
#include "ofVboMesh.h"
#include "ofTypes.h"
#include "ofShader.h"
#include "vrUtils.h"

namespace ofxOpenVrUtil {
	class GizmoComponent {
	public:
		GizmoComponent() : transform(glm::identity<glm::mat4>()), baseColor(0.9f) {}

		virtual void setup() = 0;

		void draw(bool showBound) {

			ofPushMatrix();
			if (bound && showBound) bound->debugDraw(); // Bouding shape is already transformed!

			ofMultMatrix(transform);
			shader.begin();
			shader.setUniform3f("lightDir", transform * glm::vec4(lightDir, 0));
			if (bSelected) {
				shader.setUniform4f("baseColor", selectColor);
			} else {
				shader.setUniform4f("baseColor", baseColor);
			}

			for (auto& m : meshes) {
				m.draw();
			}
			shader.end();
			ofPopMatrix();
		}

		bool checkInside(const glm::vec3& p) const {
			return bound->isInside(glm::vec4(p, 1.f));
		}

		void setSelected(bool s) { bSelected = s; }
		bool isSelected() const { return bSelected; }

		void setTransform(const glm::mat4& m) {
			transform = m;
			center = m[3];
			bound->transform(transform);
		}
		const glm::mat4& getTransform() const { return transform; }

		void setColor(const ofFloatColor& col) {
			baseColor = col;
		}

	protected:
		bool bSelected;
		glm::mat4 transform;
		glm::vec3 center;
		ofPtr<BoundingShape> bound;
		ofFloatColor baseColor;

		static ofShader shader;
		const static ofFloatColor selectColor;
		const static glm::vec3 lightDir;

		std::vector<ofVboMesh> meshes;
	};

	class GizmoArrow : public GizmoComponent {
	public:
		void setup() override {
			if (!shader.isLoaded()) shader.load(shaderPath + "lambert");

			std::array<ofVboMesh, 2> ms;
			ms[0] = ofMesh::cone(30.f, 100.f, 24, 1);
			glm::mat4 m = glm::scale(glm::vec3(0.001f)) * glm::translate(glm::vec3(0, 200, 0)) * glm::scale(glm::vec3(1, -1, 1));
			for (auto& v : ms[0].getVertices()) {
				v = m * glm::vec4(v, 1.f);
			}

			ms[1] = ofMesh::cylinder(10.f, 200.f, 12, 2);
			m = glm::scale(glm::vec3(0.001f)) * glm::translate(glm::vec3(0, 100.f, 0));
			for (auto& v : ms[1].getVertices()) {
				v = m * glm::vec4(v, 1.f);
			}

			// Concate 2 mesh's vectors
			std::vector<glm::vec3> vertList(ms[0].getVertices());
			std::copy(ms[1].getVertices().begin(), ms[1].getVertices().end(), std::back_inserter(vertList));

			meshes.push_back(std::move(ms[0]));
			meshes.push_back(std::move(ms[1]));

			ofPtr<BoundingBox> bb = std::make_shared<BoundingBox>();
			bb->fit(vertList);
			bb->setOffset(0.01f);

			bound = bb;
		}

	private:

	};

	class GizmoSphere : public GizmoComponent {
	public:
		void setup() override {

			if (!shader.isLoaded()) shader.load(shaderPath + "lambert");

			float radius = 0.03f;
			ofVboMesh sphere = ofMesh::sphere(radius, 8);
			meshes.push_back(std::move(sphere));

			ofPtr<BoundingSphere> bs = std::make_shared<BoundingSphere>();
			bs->setRadius(radius);
			bs->setOffset(0.01f);

			bound = bs;
		}

	private:

	};

	class GizmoArc : public GizmoComponent {
	public:
		void setup() override {

			if (!shader.isLoaded()) shader.load(shaderPath + "lambert");

			ofVboMesh arc;
			int radRes = 12;
			float outerRad = 0.2f;
			float innerRad = 0.16f;

			for (int i = 0; i <= radRes; i++) {
				float t = HALF_PI * float(i) / float(12) - HALF_PI;

				arc.addVertex(glm::vec3(outerRad * sin(t), 0, outerRad * cos(t)));
				arc.addVertex(glm::vec3(innerRad * sin(t), 0, innerRad * cos(t)));

			}
			arc.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

			ofPtr<BoundingBox> bb = std::make_shared<BoundingBox>();
			bb->fit(arc.getVertices());
			bb->setOffset(0.03f);
			bound = bb;

			meshes.push_back(std::move(arc));
		}

	private:
	};
}