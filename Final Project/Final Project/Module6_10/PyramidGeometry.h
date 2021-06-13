#pragma once

class PyramidGeometry : public GeometryNode {
public:
	/**
	 * Constructor.
	 */
	PyramidGeometry(const float width, const int position_loc, const int normal_loc, const int texture_loc) {
		//TexturedTriSurface();
		baseVerts[0] = PNTVertex(Point3(-width * 0.5f, -width * 0.5f, 0.0f));
		baseVerts[1] = PNTVertex(Point3(width * 0.5f, -width * 0.5f, 0.0f));
		baseVerts[2] = PNTVertex(Point3(width * 0.5f, width * 0.5f, 0.0f));
		baseVerts[3] = PNTVertex(Point3(-width * 0.5f, width * 0.5f, 0.0f));

		baseVerts[0].normal = Vector3(-0.5f, -0.5f, 0.0f);
		baseVerts[1].normal = Vector3(0.5f, -0.5f, 0.0f);
		baseVerts[2].normal = Vector3(0.5f, 0.5f, 0.0f);
		baseVerts[3].normal = Vector3(-0.5f, 0.5f, 0.0f);


		baseVerts[0].normal = baseVerts[0].normal.Normalize();
		baseVerts[1].normal = baseVerts[1].normal.Normalize();
		baseVerts[2].normal = baseVerts[2].normal.Normalize();
		baseVerts[3].normal = baseVerts[3].normal.Normalize();

		baseVerts[0].s = 0.0f; baseVerts[0].t = 0.0f;
		baseVerts[1].s = 5.0f; baseVerts[1].t = 0.0f;
		baseVerts[2].s = 0.0f; baseVerts[2].t = 0.0f;
		baseVerts[3].s = 5.0f; baseVerts[3].t = 0.0f;


		Matrix4x4 scaleVertex;
		scaleVertex.m00() = 0.01f;
		scaleVertex.m11() = 0.01f;
		scaleVertex.m22() = 1.0f;
		scaleVertex.m33() = 1.0f;

		for (int i = 0; i < 4; i++) {
			topVerts[i].vertex = baseVerts[i].vertex + Vector3(0.0f, 0.0f, width * 0.5f);

			topVerts[i].vertex = scaleVertex * topVerts[i].vertex;
			topVerts[i].s = baseVerts[i].s;
			topVerts[i].t = baseVerts[i].t + 5.0f;
			topVerts[i].normal = baseVerts[i].normal;
		}




		CreateVertexBuffers(position_loc, normal_loc, texture_loc);
	}

	void CreateVertexBuffers(const int position_loc, const int normal_loc, const int texture_loc) {
		// Generate vertex buffers for the vertex list and the face list
		for (int i = 0; i < 4; i++) {
			int nextIndex = (i + 1) % 4;
			extrusionVerts.push_back(baseVerts[i]);
			extrusionVerts.push_back(baseVerts[nextIndex]);
			extrusionVerts.push_back(topVerts[i]);

			extrusionVerts.push_back(topVerts[i]);
			extrusionVerts.push_back(baseVerts[nextIndex]);
			extrusionVerts.push_back(topVerts[nextIndex]);
		}

		/*extrusionVerts.push_back(baseVerts[0]);
		extrusionVerts.push_back(baseVerts[2]);
		extrusionVerts.push_back(baseVerts[1]);*/

		glGenBuffers(1, &vboT);
		glBindBuffer(GL_ARRAY_BUFFER, vboT);
		
		glGenVertexArrays(1, &vaoT);
		glBindVertexArray(vaoT);

		// Bind the vertex buffer, set the vertex position attribute and the vertex normal attribute
		glBindBuffer(GL_ARRAY_BUFFER, vboT);
		glBufferData(GL_ARRAY_BUFFER, extrusionVerts.size() * sizeof(PNTVertex),
			(void*)&extrusionVerts.front(), GL_STATIC_DRAW);
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


	/**
	* Draw this geometry node.
	*/
	virtual void Draw(SceneState& scene_state) {

		int error = glGetError();
		glBindVertexArray(vaoT);
		glDrawArrays(GL_TRIANGLES, 0, extrusionVerts.size());
		glBindVertexArray(0);
		error = glGetError();

		int x = 5;
	}

protected:

	GLuint   vaoT;
	GLuint   vboT;
	PNTVertex baseVerts[4], topVerts[4];
	std::vector<PNTVertex> extrusionVerts;
};