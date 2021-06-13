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
class ObjGeometry : public GeometryNode {
public:
	/**
	* Constructor.
	*/
	ObjGeometry()
		: GeometryNode() {
		vao = 0;
		vbo = 0;
	}

	/**
	* Destructor.
	*/
	~ObjGeometry() {
		// Delete vertex buffer objects
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	/**
	* Draw this geometry node.
	*/
	virtual void Draw(SceneState& scene_state) {

		int error = glGetError();


		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glBindVertexArray(0);
		error = glGetError();

	}

	/**
	* Construct triangle surface by passing in vertex list and face list
	* @param  v  List of vertices (position and normal)
	* @param  f    Index list for triangles
	*/
	void Construct(std::vector<PNTVertex>& v, std::vector<uint16_t>& f,
				   const int position_loc, const int normal_loc, const int texture_loc) {
		vertices = v;
		faces = f;
		

		// Create the vertex and face buffers
		CreateVertexBuffers(position_loc, normal_loc, texture_loc);
	}

	/**
	* Creates vertex buffers for this object.
	*/
	void CreateVertexBuffers(const int position_loc, const int normal_loc, const int texture_loc) {
		// Generate vertex buffers for the vertex list and the face list
		glGenBuffers(1, &vbo);

		// Bind the vertex list to the vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PNTVertex),
			(void*)&vertices[0], GL_STATIC_DRAW);

		// Bind the vertex buffer, set the vertex position attribute and the vertex normal attribute
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Allocate a VAO, enable it and set the vertex attribute arrays and pointers
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(PNTVertex), (void*)0);
		glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(PNTVertex), (void*)(sizeof(Point3)));
		glVertexAttribPointer(texture_loc, 2, GL_FLOAT, GL_FALSE, sizeof(PNTVertex),
			(void*)(sizeof(Point3) + sizeof(Vector3)));
		glEnableVertexAttribArray(position_loc);
		glEnableVertexAttribArray(normal_loc);
		glEnableVertexAttribArray(texture_loc);




		// Make sure changes to this VAO are local
		glBindVertexArray(0);


	}

protected:
	// Vertex buffer support
	GLuint vao;
	GLuint vbo;

	// Vertex and normal list
	std::vector<PNTVertex> vertices;

	// Use uint16_t for face list indexes (OpenGL ES compatible)
	std::vector<uint16_t> faces;



};
