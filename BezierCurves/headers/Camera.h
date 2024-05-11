#pragma once
// Local Headers
#include "Vertex.h"
#include "Transform.h"
#include "Mesh.h"
#include "Object.h"
#include "Renderer.h"

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


class Camera : public Transform {
public:
	// pozicija kamere u globalnom koordinatnom sustavu
	glm::vec3 camPosition;
	// tocka u koju kamera gledaa
	glm::vec3 camTarget;
	// smjer u kojem kamera gleda
	glm::vec3 camDirection;
	// globalni Up-axis (koristi se za racunanje right i up axisa kamere)
	glm::vec3 globalUp;
	// lokalni right-axis
	glm::vec3 camRight;
	// lokalni up-axis
	glm::vec3 camUp;
	// brzina kamere
	float camSpeed;
	// parametri za racunanje eulerovih kutova
	float pitch = 0.0f;
	float yaw = -90.0f;

	// konstruktor
	Camera(glm::vec3 camPosition, glm::vec3 camTarget, glm::vec3 globalUp, float camSpeed);

	void updateCamDirection();

	glm::mat4 getPerspectiveMatrix(float fov, int width, int height);

	glm::mat4 getViewMatrix();
};