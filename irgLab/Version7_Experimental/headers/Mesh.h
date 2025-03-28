#pragma once

// Local Headers
#include <Vertex.h>
#include <Texture.h>
#include <Transform.h>
#include <Material.h>

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



class Mesh {
public:
	// podatci o meshu
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	GLuint VAO, VBO, EBO;

	// konstruktor
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	std::vector<float> getBoundingBox();

	void applyTransform(Transform transform);

	void normalizeMesh(Transform transform, Transform totalTransform);

	// inicijaliziraj sve buffere/arrayeve
	void setupMesh();

	void drawMesh(Shader& shader, Transform transform, Material material);
};