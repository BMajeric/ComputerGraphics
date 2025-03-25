#pragma once

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



class Mesh {
public:
	// podatci o meshu
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	GLuint VAO, VBO, EBO;
	Transform transform;

	// konstruktor
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	std::vector<float> getBoundingBox();

	void applyTransform();

	void normalizeMesh();

	// inicijaliziraj sve buffere/arrayeve
	void setupMesh();

	void drawMesh(Shader& shader);

private:
	// inicijaliziraj transformu na jedinicnu matricu (ne postoji pocetna translacija niti rotacija te objekt nije skaliran)
	Transform initializeTransform();
};