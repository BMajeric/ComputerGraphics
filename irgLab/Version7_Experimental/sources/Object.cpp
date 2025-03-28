// Local Headers
#include <Vertex.h>
#include <Texture.h>
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


#include "stb_image.h"



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
	std::vector<Texture> textures;


	// prodi po svakom vertexu mesha i spremi njegove vrijednosti u nas vertex koji ce ici u nas mesh
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vector;
		glm::vec3 normalVector;
		glm::vec2 textureVector;

		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		normalVector.x = mesh->mNormals[i].x;
		normalVector.y = mesh->mNormals[i].y;
		normalVector.z = mesh->mNormals[i].z;
		vertex.Normal = normalVector;

		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);

	}

	// indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		// spremi indekse u indices vektor
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// textures
	aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

	for (unsigned int i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); i++) {
		aiString str;
		mat->GetTexture(aiTextureType_DIFFUSE, i, &str);
		Texture texture;
		texture.id = TextureFromFile(str.C_Str(), this->directory);
		texture.type = "texture_diffuse";
		texture.path = str.C_Str();
		textures.push_back(texture);
	}

	return Mesh(vertices, indices, textures);

}

unsigned int Object::TextureFromFile(const char* path, const std::string& directory) {
	std::string filename = std::string(path);
	std::string dirPath(directory, 0, directory.find_last_of("\\/"));
	filename = dirPath + '\\' + filename;
	std::cout << "[filename]: " << filename << std::endl;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {

		std::cout << "TIKI TIKI" << std::endl;

		GLenum format;
		if (nrComponents == 1) {
			format = GL_RED;
		}
		else if (nrComponents == 3) {
			format = GL_RGB;
		}
		else if (nrComponents == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		std::cout << "TIKI TIKI 2.0, new and improved" << std::endl;

	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;

}

// inicijaliziraj transformu na jedinicnu matricu (ne postoji pocetna translacija niti rotacija te objekt nije skaliran)
Transform Object::initializeTransform() {
	glm::mat4 initialisedTransform = glm::mat4(1.0f);

	return Transform(initialisedTransform, initialisedTransform, initialisedTransform);
}
