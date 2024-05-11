// Local Headers
#include "Vertex.h"
#include "Transform.h"
#include "Mesh.h"
#include "Object.h"
#include "Renderer.h"
#include "Camera.h"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>


// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>

#include "Shader.h"
#include "FPSManager.h"



Camera::Camera(glm::vec3 camPosition, glm::vec3 camTarget, glm::vec3 globalUp, float camSpeed) {

	this->camPosition = camPosition;
	this->camTarget = camTarget;
	this->globalUp = globalUp;
	this->viewMat = glm::lookAt(camPosition, camPosition + camDirection, globalUp);
	this->camSpeed = camSpeed;

	camDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	camDirection.y = sin(glm::radians(pitch));
	camDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	camRight = glm::normalize(glm::cross(globalUp, camDirection));
	camUp = glm::normalize(glm::cross(camDirection, camRight));

}

void Camera::updateCamDirection() {

	glm::vec3 rawCamDirection;
	rawCamDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	rawCamDirection.y = sin(glm::radians(pitch));
	rawCamDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	camDirection = glm::normalize(rawCamDirection);

	camRight = glm::normalize(glm::cross(globalUp, camDirection));

	camUp = glm::normalize(glm::cross(camDirection, camRight));

}

glm::mat4 Camera::getPerspectiveMatrix(float fov, int width, int height) {

	glm::mat4 perspectiveMat = glm::perspective(fov, (float)width / (float)height, 0.1f, 100.0f);

	return perspectiveMat;

}

glm::mat4 Camera::getViewMatrix() {
	viewMat = glm::lookAt(camPosition, camPosition + camDirection, camUp);
	return viewMat;
}

