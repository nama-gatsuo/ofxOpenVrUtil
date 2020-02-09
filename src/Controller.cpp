#include "Controller.h"
#include "ofPixels.h"
#include "ofGraphics.h"

namespace ofxOpenVrUtil {
	Controller::Controller(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role, ofPtr<Model> model) :
		deviceIndex(deviceIndex), role(role), model(model)
	{}
	void Controller::draw() {
		ofPushMatrix();
		ofMultMatrix(transform);
		model->tex.bind();
		model->mesh.draw();
		model->tex.unbind();
		ofPopMatrix();
	}
	
	
	void ControllerManager::setup(vr::IVRSystem* vrSys) {
		this->vrSys = vrSys;
	}
	bool ControllerManager::hasDevice(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role) const {
		if (!hasDevices()) return false;
		for (auto& c : controllers) {
			if (c.second->deviceIndex == deviceIndex && c.second->role == role) {
				return true;
			}
		}
		return false;
	}
	void ControllerManager::addDevice(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role) {
		if (models.find(role) == models.end()) {
			// If don't have a certain model for ControllerRole, let us load model for it
			loadModel(deviceIndex, role);
		}
		controllers[deviceIndex] = std::make_shared<Controller>(deviceIndex, role, models[role]);
	}
	void ControllerManager::loadModel(vr::TrackedDeviceIndex_t deviceIndex, vr::ETrackedControllerRole role) {
		
		std::string modelName = getPropString(vrSys, deviceIndex, vr::Prop_RenderModelName_String);
		vr::RenderModel_t* renderModel = nullptr;

		// Polling in every 1ms to async function 
		vr::EVRRenderModelError error;
		while (true) {
			error = vr::VRRenderModels()->LoadRenderModel_Async(modelName.c_str(), &renderModel);
			if (error != vr::VRRenderModelError_Loading) break;
			ofSleepMillis(1);
		}

		if (error != vr::VRRenderModelError_None) {
			ofLogError(__FUNCTION__) << "Unable to create model from render model";
			vr::VRRenderModels()->FreeRenderModel(renderModel);
			return;
		}

		vr::RenderModel_TextureMap_t* renderModelTex;
		while (true) {
			error = vr::VRRenderModels()->LoadTexture_Async(renderModel->diffuseTextureId, &renderModelTex);
			if (error != vr::VRRenderModelError_Loading) break;
			ofSleepMillis(1);
		}

		if (error != vr::VRRenderModelError_None) {
			ofLogError(__FUNCTION__) << "Unable to load render texture of: " << modelName;
			vr::VRRenderModels()->FreeRenderModel(renderModel);
			vr::VRRenderModels()->FreeTexture(renderModelTex);
			return;
		}

		ofPtr<Model> m = std::make_shared<Model>();
		m->tex.allocate(renderModelTex->unWidth, renderModelTex->unHeight, GL_RGBA8, false);
		m->tex.loadData(renderModelTex->rubTextureMapData, renderModelTex->unWidth, renderModelTex->unHeight, GL_RGBA);

		m->mesh.setMode(OF_PRIMITIVE_TRIANGLES);
		for (int i = 0; i < renderModel->unVertexCount; i++) {
			auto& v = renderModel->rVertexData[i];
			m->mesh.addVertex(toGlm(v.vPosition));
			m->mesh.addNormal(toGlm(v.vNormal));
			ofLogNotice() << toGlm(v.rfTextureCoord);
			m->mesh.addTexCoord(toGlm(v.rfTextureCoord));
		}
		for (int i = 0; i < renderModel->unTriangleCount; i++) {
			m->mesh.addIndex(renderModel->rIndexData[i * 3 + 0]);
			m->mesh.addIndex(renderModel->rIndexData[i * 3 + 1]);
			m->mesh.addIndex(renderModel->rIndexData[i * 3 + 2]);
		}

		vr::VRRenderModels()->FreeRenderModel(renderModel);
		vr::VRRenderModels()->FreeTexture(renderModelTex);
		models[role] = m;
		
		ofLogNotice(__FUNCTION__) << "Model ( name: " << modelName << ", vertex: " << m->mesh.getNumVertices() << " ) was loaded!";
	}
}