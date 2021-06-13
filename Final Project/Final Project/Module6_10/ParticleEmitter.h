#pragma once
#include <iostream>
#include <vector>

#include <GL/gl3w.h>
#include <GL/freeglut.h>
#include "geometry/geometry.h"
#include "shader_support/glsl_shader.h"
#include "scene/scene.h"

class ParticleEmitter : public SceneNode
{

public:
	struct PositionAndColor
	{
		Vector3 color;
		Vector3 position;
	};
	struct Particle
	{
		PositionAndColor pandC;
		Vector3 velocity;
		float timeAlive;
		float currTime;

	};

	static const int MAX_PARTICLES = 400;
	Vector3 cubeCenter = Vector3(0, 0, 0);
	std::vector<Particle> particles;
	Vector3 defaultColor;

	//update the position for each particlke
	void update(float delaTime)
	{
		for (unsigned int i = 0; i < particles.size(); ++i)
		{
			Particle& particle = particles[i];

			particle.currTime += delaTime;
			if (particle.currTime > particle.timeAlive)
			{
				randomizeParitcle(&particle);
			}
			else
			{
				particle.pandC.position += (particle.velocity * delaTime);
			}
		}
	}

private:
	
	GLuint vao;
	GLuint vbo;

	// Create a random value between a specified minv and maxv.
	float getRandom(const float minv, const float maxv) {
		return minv + ((maxv - minv) * (float)rand() / (float)RAND_MAX);
	}

	//randomize the velocity and position for a new particle
	void randomizeParitcle(Particle *p)
	{
		//int x = getRandom(-100, 100);
		int x = getRandom(-100, 100);
		int y = getRandom(-100, 100);
		int z = getRandom(-100, 100);
		int speed = getRandom(2, 5);

		int xVel = getRandom(-10, 10);
		int yVel = getRandom(-10, 10);
		int zVel = getRandom(0, 10); 

		p->pandC.position = Vector3(x, y, z);
		p->pandC.color = defaultColor;
		p->timeAlive = getRandom(40, 50);
		p->currTime = 0;
		p->velocity = Vector3(xVel, yVel, zVel).Normalize() * speed;


	}

	

     void Draw(SceneState& scene_state)
	{
		 
		glPointSize(8.0f);
		glUniformMatrix4fv(scene_state.pv_loc, 1, GL_FALSE, scene_state.pv.Get());
	    glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, particles.size());
		glBindVertexArray(0);
	}


	/**
	* Creates vertex buffers for this object.
	*/
	void CreateVertexBuffers( int position_loc,  int color_loc) {
		

		// Bind the vertex buffer, set the vertex position attribute and the vertex normal attribute
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), (void*)&particles.front(), GL_STATIC_DRAW);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glEnableVertexAttribArray(color_loc);
		glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);

		glEnableVertexAttribArray(position_loc);
		glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(sizeof(Vector3)));
	
		glBindVertexArray(0);
	}
public:
	//Constructor
	ParticleEmitter( int position_loc,  int color_loc)
	{
		
		glGenVertexArrays(1, &vbo);
		glGenVertexArrays(1, &vao);
		defaultColor = Vector3(0.76f, 0.69f, 0.501f);
		for (int i = 0; i < MAX_PARTICLES; i++)
		{
			Particle p;
			randomizeParitcle(&p);
			particles.push_back(p);
			
		}

		CreateVertexBuffers(position_loc, color_loc);
	}

	/**
	* Update the scene node and its children
	*/
	virtual void Update(SceneState& scene_state) {
		// Check if intersection occured
		update(1);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle),
			(GLvoid*)&particles.front());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Update all children
		SceneNode::Update(scene_state);
	}
};