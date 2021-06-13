#pragma once
class SkyboxPresentation : public SceneNode {
public:
	SkyboxPresentation()
	{

	}


	// utility function for loading a 2D texture from file
	// ---------------------------------------------------
	unsigned int loadTexture(char const * path)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		// Bind a DevIL image
		ILuint id;
		ilGenImages(1, &id);
		ilBindImage(id);
		ILuint err = ilGetError();
		if (err) {
			printf("Error binding image. %s %d\n", path, err);
			return false;
		}

		// Load image using lower left origin. Convert to RGBA. Tyr loading from 
		// ../textures, if that fails try ../../textures
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
		ilEnable(IL_ORIGIN_SET);
		std::string full_path_name = "../textures/" + (std::string)path;
		ilLoadImage(full_path_name.c_str());
		err = ilGetError();
		if (err) {
			full_path_name = "../../textures/" + (std::string)path;
			ilLoadImage(full_path_name.c_str());
		}
		err = ilGetError();
		if (err) {
			printf("Error loading texture. %s %d\n", path, err);
			return false;
		}
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		err = ilGetError();
		if (err) {
			printf("Could not convert texture to RGBA. %s %d\n", path, err);
			return false;
		}

		// Get image dimensions and data
		int w = ilGetInteger(IL_IMAGE_WIDTH);
		int h = ilGetInteger(IL_IMAGE_HEIGHT);

		unsigned char* data = ilGetData();
		if (data)
		{


			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			//free(data);
		}

		return textureID;
	}

	// loads a cubemap texture from 6 individual texture faces
	// order:
	// +X (right)
	// -X (left)
	// +Y (top)
	// -Y (bottom)
	// +Z (front) 
	// -Z (back)
	// -------------------------------------------------------
	unsigned int loadCubemap(std::vector<std::string> faces)
	{
		
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			// Bind a DevIL image
			ILuint id;
			ilGenImages(1, &id);
			ilBindImage(id);
			ILuint err = ilGetError();
			if (err) {
				printf("Error binding image. %s %d\n", faces[i].c_str(), err);
				return false;
			}

			// Load image using lower left origin. Convert to RGBA. Tyr loading from 
			// ../textures, if that fails try ../../textures
			ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
			//ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
			ilEnable(IL_ORIGIN_SET);
			std::string full_path_name = "../textures/" + (std::string)faces[i].c_str();
			ilLoadImage(full_path_name.c_str());
			err = ilGetError();
			if (err) {
				full_path_name = "../../textures/" + (std::string)faces[i].c_str();
				ilLoadImage(full_path_name.c_str());
			}
			err = ilGetError();
			if (err) {
				printf("Error loading texture. %s %d\n", faces[i].c_str(), err);
				return false;
			}
			ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
			err = ilGetError();
			if (err) {
				printf("Could not convert texture to RGBA. %s %d\n", faces[i].c_str(), err);
				return false;
			}

			// Get image dimensions and data
			int w = ilGetInteger(IL_IMAGE_WIDTH);
			int h = ilGetInteger(IL_IMAGE_HEIGHT);
			unsigned char *data = ilGetData();

			if (ilGetError() != IL_NO_ERROR) {
				std::cout << "Error getting image data" << std::endl;
			}

			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				//free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				//free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		//glBindTexture(GL_TEXTURE_2D, 0);
		return textureID;
	}

	void Draw(SceneState& scene_state) {
		// Set the material uniform values
		

		Matrix4x4 viewTemp = scene_state.view;

		viewTemp.m03() = 0;
		viewTemp.m13() = 0;
		viewTemp.m23() = 0;
		viewTemp.m33() = 1;

		int error = glGetError();

		// Set the matrices
		glUniformMatrix4fv(scene_state.projection_loc, 1, GL_FALSE, scene_state.projection.Get());
		error = glGetError();
		glUniformMatrix4fv(scene_state.view_loc, 1, GL_FALSE, viewTemp.Get());
		error = glGetError();
	
		glDepthFunc(GL_LEQUAL);
		error = glGetError();
		//glBindVertexArray(1);
		//glActiveTexture(GL_TEXTURE0 + 0);
		//glBindTexture(GL_TEXTURE_2D, textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		error = glGetError();
		//glBindTexture(GL_TEXTURE_2D, 0);

		// Draw children of this node
		SceneNode::Draw(scene_state);
		glDepthFunc(GL_LESS);
	}

	public:

		GLuint textureID;
};