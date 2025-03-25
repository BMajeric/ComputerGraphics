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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int width = 800, height = 500;
double boje[3] = { 0, 0, 0 };

//malo je nespretno napravljeno jer ne koristimo c++17, a treba podrzati i windows i linux,
//slobodno pozivajte new Shader(...); direktno
Shader* loadShader(char* path, char* naziv) {
	std::string sPath(path);
	std::string pathVert;
	std::string pathFrag;

	pathVert.append(path, sPath.find_last_of("\\/") + 1);
	pathFrag.append(path, sPath.find_last_of("\\/") + 1);
	if (pathFrag[pathFrag.size() - 1] == '/') {
		pathVert.append("shaders/");
		pathFrag.append("shaders/");
	}
	else if (pathFrag[pathFrag.size() - 1] == '\\') {
		pathVert.append("shaders\\");
		pathFrag.append("shaders\\");
	}
	else {
		std::cerr << "nepoznat format pozicije shadera";
		exit(1);
	}

	pathVert.append(naziv);
	pathVert.append(".vert");
	pathFrag.append(naziv);
	pathFrag.append(".frag");

	return new Shader(pathVert.c_str(), pathFrag.c_str());
}


//funkcija koja se poziva prilikom mijenjanja velicine prozora, moramo ju povezati pomocu glfwSetFramebufferSizeCallback
void framebuffer_size_callback(GLFWwindow* window, int Width, int Height)
{
	width = Width;
	height = Height;

	glViewport(0, 0, width, height);

}

int main(int argc, char* argv[]) {
	// kreiranje prozora

	GLFWwindow* window;

	glfwInit();

	window = glfwCreateWindow(width, height, "Zadatak 4", nullptr, nullptr);
	// provjeri je li se uspio napraviti prozor
	if (window == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}
	// kontekst mora biti postavljen prije ostalih stvari inace ce nam VBO poluditi i rec da je NULL pointer
	glfwMakeContextCurrent(window);

	gladLoadGL();

	// dohvati sve dostupne OpenGL funkcije
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to initialize GLAD");
		exit(-1);
	}
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	glClearColor(1, 1, 1, 1); //boja brisanja platna izmedu iscrtavanja dva okvira

	glfwSwapInterval(0); //ne cekaj nakon iscrtavanja (vsync)

	// pronalazenje patha do .obj filea
	std::string path(argv[0]);
	std::string dirPath(path, 0, path.find_last_of("\\/"));
	std::string resPath(dirPath);
	resPath.append("\\resources"); //za linux pretvoriti u forwardslash
	std::string objPath(resPath);
	objPath.append("\\dragon.obj"); //za linux pretvoriti u forwardslash

	std::cout << objPath << std::endl;

	FPSManager FPSManagerObject(window, 120, 1.0, "Zadatak 4");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //funkcija koja se poziva prilikom mijenjanja velicine prozora

	// loadanje shadera sjencar kojeg predajemo iscrtavanju
	Shader* sjencar[1];
	sjencar[0] = loadShader(argv[0], "shader");

	//uniformna varijabla za boju iscrtavanja mesha
	GLint lokacijaUniformVarijableZaBoju = glGetUniformLocation(sjencar[0]->ID, "u_color");

	// pripremanje renderera za iscrtavanje objekta
	std::vector<Object> objects;
	Object model = Object(objPath);
	objects.push_back(model);
	Renderer renderer = Renderer(objects);

	for (auto mesh : objects[0].meshes) {
		mesh.normalizeMesh();
		std::vector<float> box = mesh.getBoundingBox();
		for (unsigned int i = 0; i < box.size(); i++) {
			std::cout << box[i] << " ";
		}
	}
	
	// glavna petlja za prikaz
	while (glfwWindowShouldClose(window) == false) {

		float deltaTime = (float)FPSManagerObject.enforceFPS(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform3f(lokacijaUniformVarijableZaBoju, boje[0], boje[1], boje[2]);


		renderer.render(*sjencar[0]);

		glfwSwapBuffers(window);
		glfwPollEvents();

		//izlazak iz programa
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

	}


	glfwTerminate();

	return EXIT_SUCCESS;

}