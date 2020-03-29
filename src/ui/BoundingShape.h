#pragma once
#include "ofVectorMath.h"
#include "ofGraphics.h"
#include "of3dGraphics.h"

namespace ofxOpenVrUtil {
	class BoundingShape {
	public:
		BoundingShape() {}
		virtual ~BoundingShape() {}

		// Need to override
		virtual bool isInside(const glm::vec3& p) const = 0;
		virtual void debugDraw() const = 0;
		virtual void transform(const glm::mat4& m) = 0;

	protected:
	};

	class BoundingSphere : public BoundingShape {
	public:
		BoundingSphere() {}
		void setOffset(float o) { offset = o; }
		void setRadius(float r) { radius = r; }
		void transform(const glm::mat4& m) override { center = m * glm::vec4(center, 1.f); }
		bool isInside(const glm::vec3& p) const override {
			return glm::length(p) < radius + offset;
		}
		void debugDraw() const override {
			ofPushStyle();
			ofNoFill();
			ofSetColor(ofColor(0, 255, 255));

			ofPushMatrix();
			ofDrawCircle(glm::vec3(0), radius);
			ofPopMatrix();

			ofPushMatrix();
			ofRotateX(90);
			ofDrawCircle(glm::vec3(0), radius);
			ofPopMatrix();

			ofPushMatrix();
			ofRotateZ(90);
			ofDrawCircle(glm::vec3(0), radius);
			ofPopMatrix();

			ofPopStyle();
		}

	private:
		glm::vec3 center;
		float radius;
		float offset;
	};

	class BoundingBox : public BoundingShape {
	public:
		BoundingBox() {}

		void setOffset(float o) { offset = o; }
		void transform(const glm::mat4& m) override {
			glm::vec3 tp0 = m * glm::vec4(minP, 1.f);
			glm::vec3 tp1 = m * glm::vec4(maxP, 1.f);

			minP = glm::min(tp0, tp1);
			maxP = glm::max(tp0, tp1);
		}

		void fit(const std::vector<glm::vec3>& verts) {
			glm::vec3 currentMin(verts[0]), currentMax(verts[0]);
			for (auto& p : verts) {
				currentMin = glm::min(currentMin, p);
				currentMax = glm::max(currentMax, p);
			}

			minP = currentMin;
			maxP = currentMax;
		}

		bool isInside(const glm::vec3& p) const override {
			return glm::all(glm::greaterThanEqual(p, minP - offset)) && glm::all(glm::lessThanEqual(p, maxP + offset));

		}
		void debugDraw() const override {
			ofPushStyle();
			ofNoFill();
			ofSetColor(ofColor(0, 255, 255));
			ofDrawBox(glm::mix(minP, maxP, 0.5f), maxP.x - minP.x, maxP.y - minP.y, maxP.z - minP.z);
			ofPopStyle();
		}

		void setMin(const glm::vec3& p) { minP = p; }
		void setMax(const glm::vec3& p) { maxP = p; }

	private:
		glm::vec3 minP, maxP;
		float offset;
	};
}