// Local Headers
#include <Vertex.h>
#include <Transform.h>
#include <Mesh.h>

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
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) : transform(initializeTransform()) {
	this->vertices = vertices;
	this->indices = indices;
	setupMesh();
}

std::vector<float> Mesh::getBoundingBox() {

	std::vector<float> vector;
	float xMax = vertices[0].Position.x; float yMax = vertices[0].Position.y; float zMax = vertices[0].Position.z;
	float xMin = vertices[0].Position.x; float yMin = vertices[0].Position.y; float zMin = vertices[0].Position.z;

	for (unsigned int i = 1; i < vertices.size(); i++) {
		if (vertices[i].Position.x < xMin)
			xMin = vertices[i].Position.x;
		else if (vertices[i].Position.x > xMax)
			xMax = vertices[i].Position.x;

		if (vertices[i].Position.y < yMin)
			yMin = vertices[i].Position.y;
		else if (vertices[i].Position.y > yMax)
			yMax = vertices[i].Position.y;

		if (vertices[i].Position.z < zMin)
			zMin = vertices[i].Position.z;
		else if (vertices[i].Position.z > zMax)
			zMax = vertices[i].Position.z;
	}

	vector.push_back(xMin);
	vector.push_back(xMax);
	vector.push_back(yMin);
	vector.push_back(yMax);
	vector.push_back(zMin);
	vector.push_back(zMax);

	return vector;

}

// applyTransform radi na nacin da transformira sve tocke u meshu 
// i onda im transformacijsku matricu resetira u jedinicnu 
// kako se ne bi neke transformacije ponovno izvodile
void Mesh::applyTransform() {
	for (unsigned int i = 0; i < vertices.size(); i++) {
		glm::vec4 vector = glm::vec4(vertices[i].Position, 1.0f);
		vector = transform.transformMat * vector;
		vertices[i].Position = glm::vec3(vector.x, vector.y, vector.z);
		//std::cout << vertices[i].Position[0] << " " << vertices[i].Position[1] << " " << vertices[i].Position[2] << std::endl;
	}
	transform.transformMat = glm::mat4(1.0f);
	//std::cout << "ja transformiram" << std::endl;
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void Mesh::normalizeMesh() {
	std::vector<float> boundingBox = getBoundingBox();
	float xOrigin, yOrigin, zOrigin;
	float deltaX, deltaY, deltaZ, maxRange;

	xOrigin = (boundingBox[0] + boundingBox[1]) / 2;
	yOrigin = (boundingBox[2] + boundingBox[3]) / 2;
	zOrigin = (boundingBox[4] + boundingBox[5]) / 2;

	deltaX = boundingBox[1] - boundingBox[0];
	deltaY = boundingBox[3] - boundingBox[2];
	deltaZ = boundingBox[5] - boundingBox[4];

	maxRange = std::max(std::max(deltaX, deltaY), deltaZ);


	glm::vec3 translationToOrigin = glm::vec3(-xOrigin, -yOrigin, -zOrigin);
	transform.translate(translationToOrigin);

	applyTransform();

	float scale = 2 / maxRange;
	glm::vec3 scaleVector = glm::vec3(scale, scale, scale);
	transform.scale(scaleVector);

	applyTransform();


	/*
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			std::cout << transform.transformMat[i][j] << " ";
		}
		std::cout << std::endl;
	}
	*/

}

// inicijaliziraj sve buffere/arrayeve
void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

}

void Mesh::drawMesh(Shader& shader) {


	//std::cout << vertices[0].Position[0] << std::endl;

	glUseProgram(shader.ID);

	glBindVertexArray(VAO);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);		// mice back facing poligone

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glEnable(GL_DEPTH_TEST); //ukljuci z spremnik (prikazuju se oni fragmenti koji su najblizi promatracu)
	glDepthFunc(GL_LESS);

	glBindVertexArray(0);

}

// inicijaliziraj transformu na jedinicnu matricu (ne postoji pocetna translacija niti rotacija te objekt nije skaliran)
Transform Mesh::initializeTransform() {
	glm::mat4 initialisedTransform = glm::mat4(1.0f);

	return Transform(initialisedTransform);
}
