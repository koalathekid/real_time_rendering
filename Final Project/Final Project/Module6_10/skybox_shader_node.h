
#ifndef __SKYBOXSHADERNODE_H
#define __SKYBOXSHADERNODE_H

#include <vector>
#include "scene/scene.h"

/**
 * Offset line shader node.
 */
class SkyBoxShaderNode : public ShaderNode {
public:
	/**
	 * Gets uniform and attribute locations.
	 */
	bool GetLocations() {
		view_matrix_loc = glGetUniformLocation(shader_program.GetProgram(), "V");
		if (view_matrix_loc < 0) {
			std::cout << "SkyboxShaderNode: Error getting view matrix location" << std::endl;
			return false;
		}
		projection_matrix_loc = glGetUniformLocation(shader_program.GetProgram(), "P");
		if (projection_matrix_loc < 0) {
			std::cout << "SkyboxShaderNode: Error getting projection matrix location" << std::endl;
			return false;
		}
		position_loc = glGetAttribLocation(shader_program.GetProgram(), "vPosition");
		if (position_loc < 0) {
			std::cout << "LineShaderNode: Error getting vertex position location" << std::endl;
			return false;
		}
		return true;
	}

	/**
	 * Draw method for this shader - enable the program and set up uniforms
	 * and vertex attribute locations
	 * @param  sceneState   Current scene state.
	 */
	virtual void Draw(SceneState& scene_state) {


		glDepthMask(GL_FALSE);
		shader_program.Use();

		int error = glGetError();

		// Enable this program

		// Set scene state locations to ones needed for this program
		scene_state.view_loc = view_matrix_loc;
		scene_state.position_loc = position_loc;
		scene_state.cubePosition_loc = projection_matrix_loc;
		

		// Draw all children
		SceneNode::Draw(scene_state);

		glDepthMask(GL_TRUE);
	}

	/**
	 * Get the location of the vertex position attribute.
	 * @return  Returns the vertex position attribute location.
	 */
	int GetPositionLoc() const {
		return position_loc;
	}

	

protected:
	// Uniform and attribute locations
	GLint view_matrix_loc;
	GLint projection_matrix_loc;
	GLint position_loc;
};

#endif