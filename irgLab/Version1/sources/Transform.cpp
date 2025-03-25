// Local Headers
#include <Vertex.h>
#include <Transform.h>

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



// konstruktor
Transform::Transform(glm::mat4 transformMat) {
	this->transformMat = transformMat;
}

void Transform::setPosition(float x, float y, float z) {

	glm::mat4 identityMat = glm::mat4(1.0f);
	glm::vec3 position = glm::vec3(x, y, z);

	transformMat = glm::translate(identityMat, position);
}

void Transform::translate(glm::vec3 translation) {
	transformMat = glm::translate(transformMat, translation);
}

void Transform::rotate(float degrees, glm::vec3 axis) {
	transformMat = glm::rotate(transformMat, glm::radians(degrees), axis);
}

void Transform::scale(glm::vec3 scale) {
	transformMat = glm::scale(transformMat, scale);
}