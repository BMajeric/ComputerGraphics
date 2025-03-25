#pragma once
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

// Assimp headers
#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h> 



class Object {
public:
	// podatci o objektu (modelu)
	std::vector<Mesh> meshes;
	std::string directory;
	Transform transform;	// trenutna transforma koja se treba updatat
	Transform totalTransform;	// potpuna transforma objekta

	// konstruktor
	Object(std::string const& path);

	void drawObject(Shader& shader);

private:
	void loadObject(std::string const& path);

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	// inicijaliziraj transformu na jedinicnu matricu (ne postoji pocetna translacija niti rotacija te objekt nije skaliran)
	Transform initializeTransform();

};