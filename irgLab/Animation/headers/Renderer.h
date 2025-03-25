#pragma once
// Local Headers
#include <Vertex.h>
#include <Transform.h>
#include <Mesh.h>
#include <Object.h>

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


class Renderer {
public:
	// podatci za renderiranje
	std::vector<Object> objects;

	// konstruktor
	Renderer(std::vector<Object> objects);

	void render(Shader& shader);
};