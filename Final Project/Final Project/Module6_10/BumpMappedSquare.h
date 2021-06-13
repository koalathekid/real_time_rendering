#pragma once
class BumpMapSquare : public GeometryNode
{
public:
	/**
	* Creates a unit length and width "flat surface".  The surface is composed of
	* triangles such that the unit length/width surface is divided into n
	* equal paritions in both x and y. Constructs a vertex list and face list
	* for the surface.
	* @param  n   Number of subdivisions in x and y
	*/
	BumpMapSquare(unsigned int n, float texture_scale, const int position_loc,
		const int normal_loc, const int texture_loc,
		const int tangent_loc, const int bitangent_loc) {


		// Only allow 250 subdivision (so it creates less that 65K vertices)
		if (n > 250)
			n = 250;

		// Normal is 0,0,1. z = 0 so all vertices lie in x,y plane.
		// Having issues with roundoff when n = 40,50 - so compare with some tolerance
		// Store in column order.
		float ds = texture_scale / n;
		float dt = texture_scale / n;
		PNTBTVertex vtx;
		vtx.normal.Set(0.0f, 0.0f, 1.0f);
		vtx.vertex.z = 0.0f;
		float spacing = 1.0f / n;
		for (vtx.vertex.y = -0.5, vtx.t = 0.0f; vtx.vertex.y <= 0.5f + kEpsilon;
			vtx.vertex.y += spacing, vtx.t += dt) {
			for (vtx.vertex.x = -0.5, vtx.s = 0.0f; vtx.vertex.x <= 0.5f + kEpsilon;
				vtx.vertex.x += spacing, vtx.s += ds) {
				vertices.push_back(vtx);
			}
		}

		// Construct face list and create vertex buffer objects
		ConstructRowColFaceList(n + 1, n + 1);
		CreateVertexBuffers(position_loc, normal_loc, texture_loc, tangent_loc, bitangent_loc);
	}

	/**
	* Draw this geometry node.
	*/
	virtual void Draw(SceneState& scene_state) {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, (GLsizei)face_count, GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
	}

	// Convenience method to get the index into the vertex list given the
	// "row" and "column" of the subdivision/grid
	uint16_t GetIndex(uint32_t row, uint32_t col, uint32_t ncols) const {
		return static_cast<uint16_t>((row*ncols) + col);
	}

	/**
	* Form triangle face indexes for a surface constructed using a double loop -
	* one can be considered rows of the surface and the other can be considered
	* columns of the surface. Assumes the vertex list is populated in "row"
	* order.
	* @param  nrows  Number of rows
	* @param  ncols  Number of columns
	*/
	void ConstructRowColFaceList(const uint32_t nrows, const uint32_t ncols) {
		for (uint32_t row = 0; row < nrows - 1; row++) {
			for (uint32_t col = 0; col < ncols - 1; col++) {
				// Divide each square into 2 triangles - make sure they are ccw.
				// GL_TRIANGLES draws independent triangles for each set of 3 vertices
				faces.push_back(GetIndex(row + 1, col, ncols));
				faces.push_back(GetIndex(row, col, ncols));
				faces.push_back(GetIndex(row, col + 1, ncols));


				calculatedTangentAndBitangent(&vertices[GetIndex(row + 1, col, ncols)],
											  &vertices[GetIndex(row, col, ncols)],
											  &vertices[GetIndex(row, col + 1, ncols)]);

				calculatedTangentAndBitangent(&vertices[GetIndex(row, col, ncols)],
											  &vertices[GetIndex(row, col + 1, ncols)],
					                          & vertices[GetIndex(row + 1, col, ncols)] );

				calculatedTangentAndBitangent(&vertices[GetIndex(row, col + 1, ncols)],
					                          &vertices[GetIndex(row + 1, col, ncols)],
					                          &vertices[GetIndex(row, col, ncols)] );



				faces.push_back(GetIndex(row + 1, col, ncols));
				faces.push_back(GetIndex(row, col + 1, ncols));
				faces.push_back(GetIndex(row + 1, col + 1, ncols));


				calculatedTangentAndBitangent(&vertices[GetIndex(row + 1, col, ncols)],
											  &vertices[GetIndex(row, col + 1, ncols)],
											  &vertices[GetIndex(row + 1, col + 1, ncols)]);

				calculatedTangentAndBitangent(&vertices[GetIndex(row, col + 1, ncols)],
											  &vertices[GetIndex(row + 1, col + 1, ncols)],
											  &vertices[GetIndex(row + 1, col, ncols)]);

				calculatedTangentAndBitangent(&vertices[GetIndex(row + 1, col + 1, ncols)],
											  &vertices[GetIndex(row + 1, col, ncols)],
											  &vertices[GetIndex(row, col + 1, ncols)]);
			}
		}
	}

	void calculatedTangentAndBitangent(PNTBTVertex *vec1, PNTBTVertex *vec2, PNTBTVertex *vec3)
	{
		Vector3 edge1 = vec2->vertex - vec1->vertex;
		Vector3 edge2 = vec3->vertex - vec1->vertex;
		Vector2 deltaUV1;
		deltaUV1.x = vec2->s - vec1->s;
		deltaUV1.y = vec3->t - vec1->t;
		Vector2 deltaUV2;
		deltaUV2.x = vec2->s - vec1->s;
		deltaUV2.y = vec3->t - vec1->t;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		Vector3 tangent1;
		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = tangent1.Normalize();

		Vector3 bitangent1;
		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = bitangent1.Normalize();
		

		vec1->tangent = tangent1;
		vec1->bitangent = bitangent1;
	}

	/**
	* Creates vertex buffers for this object.
	*/
	void CreateVertexBuffers(const int position_loc, const int normal_loc, const int texture_loc, const int tangent_loc, const int bitangent_loc) {
		// Generate vertex buffers for the vertex list and the face list
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &facebuffer);

		// Bind the vertex list to the vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PNTBTVertex),
			(void*)&vertices[0], GL_STATIC_DRAW);

		// Bind the face list to the vertex buffer object
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facebuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(uint16_t),
			(void*)&faces[0], GL_STATIC_DRAW);

		// Copy the face list count for use in Draw (then we can clear the vector)
		face_count = faces.size();

		// We could clear any local memory as it is now in the VBO. However there may be
		// cases where we want to keep it (e.g. collision detection, picking) so I am not
		// going to do that here.

		// Allocate a VAO, enable it and set the vertex attribute arrays and pointers
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Bind the vertex buffer, set the vertex position attribute and the vertex normal attribute
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(PNTBTVertex), (void*)0);
		glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(PNTBTVertex), (void*)(sizeof(Point3)));
		glVertexAttribPointer(texture_loc, 2, GL_FLOAT, GL_FALSE, sizeof(PNTBTVertex),
			(void*)(sizeof(Point3) + sizeof(Vector3)));

		glVertexAttribPointer(tangent_loc, 3, GL_FLOAT, GL_FALSE, sizeof(PNTBTVertex),
			(void*)(sizeof(Point3) + sizeof(Vector3) + sizeof(float)*2));
		glVertexAttribPointer(bitangent_loc, 3, GL_FLOAT, GL_FALSE, sizeof(PNTBTVertex),
			(void*)(sizeof(Point3) + sizeof(Vector3) + sizeof(float) * 2 + sizeof(Vector3)));

		glEnableVertexAttribArray(position_loc);
		glEnableVertexAttribArray(normal_loc);
		glEnableVertexAttribArray(texture_loc);
		glEnableVertexAttribArray(tangent_loc);
		glEnableVertexAttribArray(bitangent_loc);

		// Bind the face list buffer and draw. Note the use of 0 offset in glDrawElements
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facebuffer);

		// Make sure changes to this VAO are local
		glBindVertexArray(0);
	}


private:
	// Make default constructor private to force use of the constructor
	// with number of subdivisions.
	BumpMapSquare() { };
protected:
	// Vertex buffer support
	uint32_t face_count;
	GLuint vao;
	GLuint vbo;
	GLuint facebuffer;

	// Vertex and normal list
	std::vector<PNTBTVertex> vertices;

	// Use uint16_t for face list indexes (OpenGL ES compatible)
	std::vector<uint16_t> faces;
};