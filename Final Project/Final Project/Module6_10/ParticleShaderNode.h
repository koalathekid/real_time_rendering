#pragma once
//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.467 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	David W. Nesbitt
//
//	Author:	David W. Nesbitt
//	File:     ngon_shader_node.h
//	Purpose:	Derived class to handle simple shaders for drawing ngons.
//
//============================================================================

#include <vector>
#include "scene/scene.h"

/**
* Offset line shader node.
*/
class ParticleShaderNode : public ShaderNode {
public:
	/**
	* Gets uniform and attribute locations.
	*/

	ParticleShaderNode()
	{

	}
	bool GetLocations() {
		int error;

		error = glGetError();
		position_loc = glGetAttribLocation(shader_program.GetProgram(), "positionT");
		if (position_loc < 0) {
			std::cout << "Error getting vertex position location" << std::endl;
			return false;
		}
		error = glGetError();
		color_loc = glGetAttribLocation(shader_program.GetProgram(), "colorT");
		if (color_loc < 0) {
			std::cout << "Error getting color location" << std::endl;
			return false;
		}

		pv_loc = glGetUniformLocation(shader_program.GetProgram(), "pv");
		if (pv_loc < 0) {
			std::cout << "Error getting projection and view matrix location" << std::endl;
			return false;
		}
		
		return true;
	}

	/**
	* Draw method for this shader - enable the program and set up uniforms
	* and vertex attribute locations
	* @param  scene_state   Current scene state.
	*/
	virtual void Draw(SceneState& scene_state) {
		// Enable this program
		shader_program.Use();

		// Set scene state locations to ones needed for this program
		//scene_state.orthomatrix_loc = orthomatrix_loc;
		scene_state.color_loc = color_loc;
		scene_state.position_loc = position_loc;
		scene_state.pv_loc = pv_loc;

		// Set the matrix (do this here for now)
		//glUniformMatrix4fv(orthomatrix_loc, 1, GL_FALSE, scene_state.ortho_matrix.Get());

		// Draw all children
		SceneNode::Draw(scene_state);
	}
public:
	/**
	* Get the vertex position attribute location
	* @return  Returns the position attribute location.
	*/
	int GetPositionLoc() const {
		return position_loc;
	}

	int GetColorLoc() const {
		return color_loc;
	}

protected:
	// Uniform and attribute locations
	GLint color_loc;
	GLint position_loc;
	GLint pv_loc;
	
};
