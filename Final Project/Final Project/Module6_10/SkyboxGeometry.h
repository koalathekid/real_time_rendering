#pragma once
#pragma once
//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.467 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	David W. Nesbitt
//
//	Author:	  David W. Nesbitt
//	File:     TransformNode.h
//	Purpose:  Scene graph geometry node indicating a triangle mesh based
//            surface. Uses indexed vertex arrays and per vertex normals.
//
//============================================================================


/**
* Triangle mesh surface. Uses indexed vertex arrays. Stores
* vertices as VertexAndNormal.
*/
class SkyboxGeometry : public GeometryNode {
public:
	/**
	* Constructor.
	*/
	SkyboxGeometry()
		: GeometryNode() {
		skyboxVAO = 0;
		skyboxVBO = 0;
		
	}

	/**
	* Destructor.
	*/
	~SkyboxGeometry() {
		// Delete vertex buffer objects
		glDeleteBuffers(1, &skyboxVBO);
		glDeleteVertexArrays(1, &skyboxVAO);

	}

	/**
	* Draw this geometry node.
	*/
	virtual void Draw(SceneState& scene_state) {



		//glDepthMask(GL_FALSE);
		glBindVertexArray(skyboxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		int error = glGetError();
		glDepthMask(GL_TRUE);

	}

	/**
	* Construct triangle surface by passing in vertex list and face list
	* @param  v  List of vertices (position and normal)
	* @param  f    Index list for triangles
	*/
	void Construct(const int cubePosition_loc) {
		setVerticies();

		// Create the vertex and face buffers
		CreateVertexBuffers( cubePosition_loc);
	}


	void SkyboxGeometry::setVerticies()
	{



		std::vector<std::string> faces{

			"arrakisday_bk.tga",

			"arrakisday_rt.tga",
			"arrakisday_lf.tga",
			"arrakisday_ft.tga",
			"arrakisday_up.tga",
			"arrakisday_dn.tga"

		};

		std::vector<float> verts = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,



			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,
			/////////////////////////////
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f

		};

		skyboxVertices = verts;

	}
	
	void CreateVertexBuffers(const int cubePosition_loc) {
		// Generate vertex buffers for the vertex list and the face list
		// cube VAO

	
	
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(float), &skyboxVertices.front(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(cubePosition_loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);

	
	}

protected:

	// skybox VAO
	GLuint skyboxVAO, skyboxVBO;


	// Vertex and normal list
	std::vector<float> skyboxVertices;
	





};
