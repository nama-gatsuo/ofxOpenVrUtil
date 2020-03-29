#pragma once
#include "ofCamera.h"
#include "ofFbo.h"
#include "ofShader.h"
#include "ofGraphics.h"
#include "of3dGraphics.h"
#include "ofPixels.h"
#include "vrUtils.h"

namespace ofxOpenVrUtil {

	class GpuRay {
	public:
		struct HitInfo{
			int id;
			glm::vec3 pos;
		};

		GpuRay() {}

		void setup() {
			cam.setFov(1.f);
			cam.setFarClip(50.f);
			cam.setNearClip(0.0001f);

			fbo.allocate(3, 3, GL_RGBA16F);
			fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

			shader.load(shaderPath + "hitCheck");
		}

		void setDirection(const glm::vec3& dir) { cam.lookAt(cam.getPosition() + dir); }
		const glm::vec3 getDirection() const { cam.getLookAtDir(); }

		void setOrigin(const glm::vec3& pos) { cam.setPosition(pos); }
		const glm::vec3 getOrigin() const { cam.getPosition(); }

		void begin() {
			ofEnableDepthTest();
			ofDisableAlphaBlending();

			fbo.begin();
			cam.begin();
			ofClear(0);
		
		}

		void setMaxIdCount(int count) {
			shader.setUniform1i("maxCount", count);
		}

		void beginObject(int id, const glm::mat4& modelMat) {
			shader.begin();
			shader.setUniform1i("id", id);
			shader.setUniformMatrix4f("modelMat", modelMat);
		}

		void endObject() {
			shader.end();
		}

		void end() {
			cam.end();
			fbo.end();

			ofDisableDepthTest();
			ofEnableAlphaBlending();

			ofFloatPixels pix;
			fbo.getTexture().readToPixels(pix);
			ofFloatColor c = pix.getColor(1, 1);

			if (c.a > 0) {
				bHit = true;
				hit.id = c.a;
				hit.pos = glm::vec3(c.r, c.g, c.b);
			} else {
				bHit = false;
			}
		
		}
	
		const ofTexture& getTexture() const {
			return fbo.getTexture();
		}
	
		void draw() const {
			if (bHit) {
				ofDrawLine(cam.getPosition(), hit.pos);
				ofDrawSphere(hit.pos, 0.01f);
			} else {
				ofDrawLine(cam.getPosition(), cam.getPosition() + cam.getLookAtDir() * 1000.f);
			}

		}

		bool hasHit() const { return bHit; }
		const HitInfo& getHit() { return hit; }

	private:
		bool bHit;
		HitInfo hit;

		ofCamera cam;
		ofFbo fbo;
		ofShader shader;
	};

}