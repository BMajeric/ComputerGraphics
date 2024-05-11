// Local Headers
#include <Vertex.h>
#include <Transform.h>
#include <Mesh.h>
#include <Object.h>
#include <Renderer.h>

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



// podatci za renderiranje
std::vector<Object> objects;

// konstruktor
Renderer::Renderer(std::vector<Object> objects) : objects(objects) {}

void Renderer::render(Shader& shader) {
	for (auto object : objects) {
		object.drawObject(shader);
	}
}
