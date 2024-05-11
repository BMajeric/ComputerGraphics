#pragma once

// Local Headers
#include <Vertex.h>

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>

#include "Shader.h"
#include "FPSManager.h"



class Transform {
public:
	glm::mat4 transformMat;
	glm::mat4 viewMat;
	glm::mat4 perspectiveMat;

	// konstruktor
	Transform(glm::mat4 transformMat, glm::mat4 viewMat, glm::mat4 perspectiveMat);

	Transform();

	void setPosition(float x, float y, float z);

	void translate(glm::mat4 matrix, glm::vec3 translation);

	void rotate(glm::mat4 matrix, float degrees, glm::vec3 axis);

	void scale(glm::mat4 matrix, glm::vec3 scale);

	glm::mat4 getViewMatrix();

	glm::mat4 getModelMatrix();
};