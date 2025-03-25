// Local Headers
#include <Vertex.h>
#include <Transform.h>
#include <Mesh.h>

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



// konstruktor
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
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
void Mesh::applyTransform(Transform transform) {
	for (unsigned int i = 0; i < vertices.size(); i++) {
		glm::vec4 vector = glm::vec4(vertices[i].Position, 1.0f);
		vector = transform.transformMat * vector;
		vertices[i].Position = glm::vec3(vector.x, vector.y, vector.z);
		//std::cout << vertices[i].Position[0] << " " << vertices[i].Position[1] << " " << vertices[i].Position[2] << std::endl;
	}
	//transform.transformMat = glm::mat4(1.0f);
	//std::cout << "ja transformiram" << std::endl;
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void Mesh::normalizeMesh(Transform transform, Transform totalTransform) {

	Transform transformTemp = transform;
	//std::cout << glm::to_string(transform.transformMat) << std::endl;

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
	transform.translate(transform.transformMat, translationToOrigin);
	totalTransform.translate(transform.transformMat, translationToOrigin);

	applyTransform(transform);
	transform = transformTemp;

	float scale = 2 / maxRange;
	glm::vec3 scaleVector = glm::vec3(scale, scale, scale);
	transform.scale(transform.transformMat, scaleVector);
	totalTransform.scale(transform.transformMat, scaleVector);

	//std::cout << glm::to_string(transform.transformMat) << std::endl;

	applyTransform(transform);
	transform = transformTemp;

	//std::cout << glm::to_string(transform.transformMat) << std::endl;

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

void Mesh::drawMesh(Shader& shader, Transform transform) {
	//applyTransform(transform);
	//std::cout << glm::to_string(transform.transformMat) << std::endl;
	//std::cout << vertices[0].Position[0] << std::endl;

	glUseProgram(shader.ID);

	glBindVertexArray(VAO);

	/*shader.setUniformMat4("model", transform.getModelMatrix());
	shader.setUniformMat4("view", transform.getViewMatrix());
	shader.setUniformMat4("projection", transform.perspectiveMat);*/

	//std::cout << "TRANSFORM-1: " << glm::to_string(transform.transformMat) << std::endl;
	//std::cout << "VIEW-1: " << glm::to_string(transform.viewMat) << std::endl;
	//std::cout << "PERSPECTIVE-1: " << glm::to_string(transform.perspectiveMat) << std::endl;


	// retrieve the matrix uniform locations
	unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
	unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
	unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");


	// pass them to the shaders (3 different ways)
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(transform.getModelMatrix()));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(transform.getViewMatrix()));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(transform.perspectiveMat));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);		// mice back facing poligone

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glEnable(GL_DEPTH_TEST); //ukljuci z spremnik (prikazuju se oni fragmenti koji su najblizi promatracu)
	glDepthFunc(GL_LESS);

	glBindVertexArray(0);

}


