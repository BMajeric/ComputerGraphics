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

// Assimp headers
#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h> 



// konstruktor
Object::Object(std::string const& path) : transform(initializeTransform()), totalTransform(initializeTransform()) {
	loadObject(path);
}

void Object::drawObject(Shader& shader) {
	for (auto mesh : meshes) {
		mesh.drawMesh(shader, transform, materials[0]);
	}
}


void Object::loadObject(std::string const& path) {

	// loadaj preko ASSIMP-a
	// procitaj file 
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path.c_str(),
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// provjera greski
	if (!scene) {
		std::cerr << importer.GetErrorString();
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);

	if (scene->HasMaterials()) {
		std::cout << "ucitana svojstva materijala" << std::endl;

		for (int i = 1; i < scene->mNumMaterials; i++) {
			std::cout << "materijal[" << i << "]:\n";

			Material material;

			// naziv materijala
			aiString name;

			std::cout << "naziv: ";
			scene->mMaterials[i]->Get(AI_MATKEY_NAME, name);
			std::cout << name.C_Str() << std::endl;

			float shininess;
			aiColor3D ambientK, diffuseK, specularK, reflectiveK, emissiveK;

			// spekularni sjaj
			scene->mMaterials[i]->Get(AI_MATKEY_SHININESS, shininess);
			material.shininess = shininess;

			// ambijentalna komponenta
			std::cout << "ambient: ";
			scene->mMaterials[i]->Get(AI_MATKEY_COLOR_AMBIENT, ambientK);
			std::cout << ambientK.r << " " << ambientK.g << " " << ambientK.b << std::endl;
			material.ambient = glm::vec3(ambientK.r, ambientK.g, ambientK.b);

			// difuzna komponenta
			std::cout << "diffuse: ";
			scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseK);
			std::cout << diffuseK.r << " " << diffuseK.g << " " << diffuseK.b << std::endl;
			material.diffuse = glm::vec3(diffuseK.r, diffuseK.g, diffuseK.b);

			// spekularna komponenta
			std::cout << "specular: ";
			scene->mMaterials[i]->Get(AI_MATKEY_COLOR_SPECULAR, specularK);
			std::cout << specularK.r << " " << specularK.g << " " << specularK.b << std::endl;
			material.specular = glm::vec3(specularK.r, specularK.g, specularK.b);

			materials.push_back(material);

		}

	}

}

void Object::processNode(aiNode* node, const aiScene* scene) {

	// prodi po svim meshevima, kreiraj ih i dodaj u objekt
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// prodi po djeci i rekuzivno obraduj svaki node
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Object::processMesh(aiMesh* mesh, const aiScene* scene) {

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// prodi po svakom vertexu mesha i spremi njegove vrijednosti u nas vertex koji ce ici u nas mesh
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vector;
		glm::vec3 normalVector;

		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		normalVector.x = mesh->mNormals[i].x;
		normalVector.y = mesh->mNormals[i].y;
		normalVector.z = mesh->mNormals[i].z;
		vertex.Normal = normalVector;

		vertices.push_back(vertex);

	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		// spremi indekse u indices vektor
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	return Mesh(vertices, indices);

}

// inicijaliziraj transformu na jedinicnu matricu (ne postoji pocetna translacija niti rotacija te objekt nije skaliran)
Transform Object::initializeTransform() {
	glm::mat4 initialisedTransform = glm::mat4(1.0f);

	return Transform(initialisedTransform, initialisedTransform, initialisedTransform);
}
