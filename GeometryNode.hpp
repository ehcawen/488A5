// Winter 2019

#pragma once

#include "SceneNode.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

	Material material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;
// ---------------------------------------------------------------------
    /* self-implemented code */
    void geoRender(bool picking, ShaderProgram &shader, glm::mat4 &view, BatchInfoMap &batchInfoMap);
    void updateShaderUniforms(bool picking, ShaderProgram &shader, glm::mat4 & viewMatrix);
};
