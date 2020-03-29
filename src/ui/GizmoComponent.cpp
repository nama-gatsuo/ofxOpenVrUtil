#include "GizmoComponent.h"


namespace ofxOpenVrUtil {
	ofShader GizmoComponent::shader;
	const ofFloatColor GizmoComponent::selectColor(1.f, 1.f, 0.f);
	const glm::vec3 GizmoComponent::lightDir(1);
}