// Local Headers
#include "Vertex.h"
#include "Transform.h"
#include "Mesh.h"
#include "Object.h"
#include "Renderer.h"
#include "Camera.h"

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

// Assimp headers
#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h> 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int width = 800, height = 500;
float lastXpos = width / 2;
float lastYpos = height / 2;

float deltas[2] = { 0.0f, 0.0f };


class Curve {
public:
	std::vector<glm::vec3> controlPolygonPoints;
	std::vector<glm::vec3> approxCurvePoints;
	std::vector<glm::vec3> interpolationCurvePoints;
	int pointNum;
	std::vector<int> bezierFactors;
	int divs;
	GLuint VAO;
	GLuint VBO[3];
	GLuint EBO;

	Curve() {
		std::cout << "Krivulja ucitana" << std::endl;
		this->divs = 10;
		this->pointNum = 0;
	}

	void loadBuffers() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(3, VBO);
		glBindVertexArray(0);
	}

	void addPoint(glm::vec3 point) {

		std::cout << "Adding a point: (" << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;

		controlPolygonPoints.push_back(point);

		pointNum = controlPolygonPoints.size();

		divs = 20 * pointNum;

		if (pointNum > 1) {
			
			computeApproxBezier();
		}

		if (pointNum > 3) {

			computeInterpolationBezier();
		}

	}

	void computeFactors(int n) {

		int a = 1;

		// priprema vektora za spremanje faktora
		for (unsigned int i = 0; i < pointNum; i++) {
			bezierFactors.push_back(0);
		}

		// racunanje faktora
		for (unsigned int i = 1; i <= n + 1; i++) {
			bezierFactors[i - 1] = a;
			a = a * (n - i + 1) / i;
		}

	}

	void computeApproxBezier() {

		int n = pointNum - 1;
		std::vector<glm::vec3> points;

		computeFactors(n);

		float bernstein, t;

		for (int i = 0; i <= divs; i++) {

			t = 1.0f / divs * i;

			glm::vec3 point = glm::vec3(0.0f, 0.0f, 0.0f);

			for (int j = 0; j <= n; j++) {

				if (j == 0) {
					bernstein = bezierFactors[j] * pow(1 - t, n);
				}
				else if (j == n) {
					bernstein = bezierFactors[j] * pow(t, n);
				}
				else {
					bernstein = bezierFactors[j] * pow(t, j) * pow(1 - t, n - j);
				}

				point.x += bernstein * controlPolygonPoints[j].x;
				point.y += bernstein * controlPolygonPoints[j].y;
				point.z += bernstein * controlPolygonPoints[j].z;

			}

			points.push_back(point);
			std::cout << "Point added: (" << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;

		}


		approxCurvePoints = points;

	}

	void computeInterpolationBezier() {

		std::vector<glm::vec3> calculatedPoints;

		glm::mat4 uniformPowerMat(1.0f);
		uniformPowerMat[0] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		uniformPowerMat[1] = glm::vec4(1.0f / 27.0f, 1.0f / 9.0f, 1.0f / 3.0f, 1.0f);
		uniformPowerMat[2] = glm::vec4(8.0f / 27.0f, 4.0f / 9.0f, 2.0f / 3.0f, 1.0f);
		uniformPowerMat[3] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		glm::mat4 inverseUniformMat = glm::inverse(uniformPowerMat);

		glm::mat4 polygonPointMat;

		for (int i = 0; i < 4; i++) {
			polygonPointMat[i] = glm::vec4(float(controlPolygonPoints[controlPolygonPoints.size() - 1 - i].x), float(controlPolygonPoints[controlPolygonPoints.size() - 1 - i].y), float(controlPolygonPoints[controlPolygonPoints.size() - 1 - i].z), 0.0f);
		}

		glm::mat4 kMat = polygonPointMat * inverseUniformMat;

		for (unsigned int i = 0; i <= divs; i++) {
			
			float t = 1.0f / divs * i;
			glm::vec3 point = glm::vec3(0.0f, 0.0f, 0.0f);

			point.x = pow(t, 3) * kMat[0].x + pow(t, 2) * kMat[1].x + t * kMat[2].x + kMat[3].x;
			point.y = pow(t, 3) * kMat[0].y + pow(t, 2) * kMat[1].y + t * kMat[2].y + kMat[3].y;
			point.x = pow(t, 3) * kMat[0].z + pow(t, 2) * kMat[1].z + t * kMat[2].z + kMat[3].z;

			calculatedPoints.push_back(point);

		}

		for (glm::vec3 point : calculatedPoints) {
			if (point.x < 1 * pow(10, -5)) {
				point.x = 0;
			}

			if (point.y < 1 * pow(10, -5)) {
				point.y = 0;
			}

			if (point.z < 1 * pow(10, -5)) {
				point.z = 0;
			}
		}

		interpolationCurvePoints = calculatedPoints;

	}

	void drawCurves(Shader& shader) {

		

		glLineWidth(3);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, controlPolygonPoints.size() * 3 * sizeof(float), &controlPolygonPoints[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);

		shader.setUniformVec3("u_color", glm::vec3(1.0, 1.0, 1.0));

		glDrawArrays(GL_LINE_STRIP, 0, controlPolygonPoints.size());

		if (pointNum > 1) {

			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

			glBufferData(GL_ARRAY_BUFFER, approxCurvePoints.size() * 3 * sizeof(float), &approxCurvePoints[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glEnableVertexAttribArray(0);

			shader.setUniformVec3("u_color", glm::vec3(0.628, 0.095, 0.990));

			glDrawArrays(GL_LINE_STRIP, 0, approxCurvePoints.size());

		}

		if (pointNum > 3) {

			glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);

			glBufferData(GL_ARRAY_BUFFER, interpolationCurvePoints.size() * 3 * sizeof(float), &interpolationCurvePoints[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glEnableVertexAttribArray(0);

			shader.setUniformVec3("u_color", glm::vec3(0.529, 0.808, 0.922));

			glDrawArrays(GL_LINE_STRIP, 0, interpolationCurvePoints.size());

		}

		glLineWidth(1);

		glBindVertexArray(0);

	}
};


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	float xposFloat = (float)xpos;
	float yposFloat = (float)ypos;

	float deltaX = xposFloat - lastXpos;
	float deltaY = yposFloat - lastYpos;

	//postavi zadnju poziciju
	lastXpos = xposFloat;
	lastYpos = yposFloat;
	//cout << xpos << " " << ypos << " | " << lastXpos << " " << lastYpos << endl;
	//u slucaju da bude prejako ili preslabo micanje kamere
	float sensitivity = 1.0f;
	deltaX *= sensitivity;
	deltaY *= sensitivity;

	deltas[0] = deltaX;
	deltas[1] = -deltaY;

}



//malo je nespretno napravljeno jer ne koristimo c++17, a treba podrzati i windows i linux,
//slobodno pozivajte new Shader(...); direktno
Shader* loadShader(char* path, char* naziv) {
	std::string sPath(path);
	std::string pathVert;
	std::string pathFrag;
	std::string pathGeom;

	pathVert.append(path, sPath.find_last_of("\\/") + 1);
	pathFrag.append(path, sPath.find_last_of("\\/") + 1);
	pathGeom.append(path, sPath.find_last_of("\\/") + 1);
	if (pathFrag[pathFrag.size() - 1] == '/') {
		pathVert.append("shaders/");
		pathFrag.append("shaders/");
		pathGeom.append("shaders/");
	}
	else if (pathFrag[pathFrag.size() - 1] == '\\') {
		pathVert.append("shaders\\");
		pathFrag.append("shaders\\");
		pathGeom.append("shaders\\");
	}
	else {
		std::cerr << "nepoznat format pozicije shadera";
		exit(1);
	}

	pathVert.append(naziv);
	pathVert.append(".vert");
	pathFrag.append(naziv);
	pathFrag.append(".frag");
	pathGeom.append(naziv);
	pathGeom.append(".geom");

	return new Shader(pathVert.c_str(), pathFrag.c_str(), pathGeom.c_str());
}


Camera camera = Camera(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1f);
Curve bezierCurve = Curve();
bool moveAlongApprox = false;
bool moveAlongInterpol = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	// dodavanje tocke za Beziera
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		bezierCurve.addPoint(camera.camPosition);
	}

	// kretanje po aproksimacijskoj krivulji
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		if (bezierCurve.pointNum > 1)
			moveAlongApprox = true;
	}

	// kretanje po interpolacijskoj krivulji
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		if (bezierCurve.pointNum > 3)
			moveAlongInterpol = true;
	}

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

	window = glfwCreateWindow(width, height, "Zadatak 7", nullptr, nullptr);
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
	objPath.append("\\kocka.obj"); //za linux pretvoriti u forwardslash

	std::cout << objPath << std::endl;

	FPSManager FPSManagerObject(window, 120, 1.0, "Zadatak 7");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //funkcija koja se poziva prilikom mijenjanja velicine prozora

	// loadanje shadera sjencar kojeg predajemo iscrtavanju
	Shader* sjencar[2];
	sjencar[0] = loadShader(argv[0], "shader1");
	sjencar[1] = loadShader(argv[0], "shader");


	// pripremanje renderera za iscrtavanje objekta
	std::vector<Object> objects;
	Object model1 = Object(objPath);
	Object model2 = Object(objPath);
	objects.push_back(model1);
	objects.push_back(model2);
	Renderer renderer = Renderer(objects);

	for (auto object : renderer.objects) {
		for (auto mesh : object.meshes) {
			mesh.normalizeMesh(object.transform, object.totalTransform);
			std::vector<float> box = mesh.getBoundingBox();
			for (unsigned int i = 0; i < box.size(); i++) {
				std::cout << box[i] << " ";
			}
		}
	}

	/*Camera camera = Camera(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1f);
	Curve bezierCurve = Curve();*/

	glm::mat4 perspectiveMatrix = camera.getPerspectiveMatrix(glm::radians(45.0f), width, height);
	glm::mat4 viewMatrix = camera.getViewMatrix(); 
	renderer.objects[0].transform.perspectiveMat = perspectiveMatrix;
	renderer.objects[0].transform.viewMat = viewMatrix;

	renderer.objects[1].transform.perspectiveMat = perspectiveMatrix;
	renderer.objects[1].transform.viewMat = viewMatrix;

	sjencar[1]->setUniformMat4("projection", perspectiveMatrix);
	sjencar[1]->setUniformMat4("view", viewMatrix);


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	float lastXpos = width / 2;
	float lastYpos = height / 2;

	glUseProgram(sjencar[0]->ID);
	//glUseProgram(sjencar[1]->ID);

	GLint eyeUniformLocation = glGetUniformLocation(sjencar[0]->ID, "eye");

	bezierCurve.loadBuffers();

	glfwSetKeyCallback(window, key_callback);

	int approxCounter = 0;
	int interpolCounter = 0;

	
	// glavna petlja za prikaz
	while (glfwWindowShouldClose(window) == false) {

		float deltaTime = (float)FPSManagerObject.enforceFPS(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// upravljanje tipkovnicom
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.camPosition += camera.camSpeed * camera.camDirection;
			camera.camTarget += camera.camSpeed * camera.camDirection;
			/*std::cout << "TRANSFORM-1: " << glm::to_string(renderer.objects[0].transform.transformMat) << std::endl;
			std::cout << "VIEW-1: " << glm::to_string(renderer.objects[0].transform.viewMat) << std::endl;
			std::cout << "PERSPECTIVE-1: " << glm::to_string(renderer.objects[0].transform.perspectiveMat) << std::endl;*/
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.camPosition -= camera.camSpeed * camera.camDirection;
			camera.camTarget -= camera.camSpeed * camera.camDirection;
			/*std::cout << "TRANSFORM-1: " << glm::to_string(renderer.objects[0].transform.transformMat) << std::endl;
			std::cout << "VIEW-1: " << glm::to_string(renderer.objects[0].transform.viewMat) << std::endl;
			std::cout << "PERSPECTIVE-1: " << glm::to_string(renderer.objects[0].transform.perspectiveMat) << std::endl;*/
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.camPosition += camera.camSpeed * glm::normalize(glm::cross(camera.camDirection, camera.camUp));
			camera.camTarget += camera.camSpeed * glm::normalize(glm::cross(camera.camDirection, camera.camUp));
			/*std::cout << "TRANSFORM-1: " << glm::to_string(renderer.objects[0].transform.transformMat) << std::endl;
			std::cout << "VIEW-1: " << glm::to_string(renderer.objects[0].transform.viewMat) << std::endl;
			std::cout << "PERSPECTIVE-1: " << glm::to_string(renderer.objects[0].transform.perspectiveMat) << std::endl;*/
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.camPosition -= camera.camSpeed * glm::normalize(glm::cross(camera.camDirection, camera.camUp));
			camera.camTarget -= camera.camSpeed * glm::normalize(glm::cross(camera.camDirection, camera.camUp));
			/*std::cout << "TRANSFORM-1: " << glm::to_string(renderer.objects[0].transform.transformMat) << std::endl;
			std::cout << "VIEW-1: " << glm::to_string(renderer.objects[0].transform.viewMat) << std::endl;
			std::cout << "PERSPECTIVE-1: " << glm::to_string(renderer.objects[0].transform.perspectiveMat) << std::endl;*/
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera.camPosition += camera.camSpeed * glm::normalize(glm::cross(camera.camDirection, camera.camRight));
			camera.camTarget += camera.camSpeed * glm::normalize(glm::cross(camera.camDirection, camera.camRight));
			/*std::cout << "TRANSFORM-1: " << glm::to_string(renderer.objects[0].transform.transformMat) << std::endl;
			std::cout << "VIEW-1: " << glm::to_string(renderer.objects[0].transform.viewMat) << std::endl;
			std::cout << "PERSPECTIVE-1: " << glm::to_string(renderer.objects[0].transform.perspectiveMat) << std::endl;*/
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			camera.camPosition -= camera.camSpeed * glm::normalize(glm::cross(camera.camDirection, camera.camRight));
			camera.camTarget -= camera.camSpeed * glm::normalize(glm::cross(camera.camDirection, camera.camRight));
			/*std::cout << "TRANSFORM-1: " << glm::to_string(renderer.objects[0].transform.transformMat) << std::endl;
			std::cout << "VIEW-1: " << glm::to_string(renderer.objects[0].transform.viewMat) << std::endl;
			std::cout << "PERSPECTIVE-1: " << glm::to_string(renderer.objects[0].transform.perspectiveMat) << std::endl;*/
		}

		

		// upravljanje misem
		glfwSetCursorPosCallback(window, mouse_callback);

		// pitch i yaw updateani za rotaciju
		camera.yaw += deltas[0];
		camera.pitch += deltas[1];
		deltas[0] = 0.0f;
		deltas[1] = 0.0f;
		//constraint na vrtnju kamere
		if (camera.pitch > 89.0f) {
			camera.pitch = 89.0f;
		}

		if (camera.pitch < -89.0f) {
			camera.pitch = -89.0f;
		}

		camera.updateCamDirection();

		viewMatrix = camera.getViewMatrix();

		sjencar[1]->setUniformMat4("view", viewMatrix);

		// create transformations
		renderer.objects[0].transform.viewMat = viewMatrix;
		renderer.objects[0].transform.perspectiveMat = camera.getPerspectiveMatrix(glm::radians(45.0f), width, height);

		//// retrieve the matrix uniform locations
		//unsigned int modelLoc = glGetUniformLocation(sjencar[0]->ID, "model");
		//unsigned int viewLoc = glGetUniformLocation(sjencar[0]->ID, "view");
		//unsigned int projectionLoc = glGetUniformLocation(sjencar[0]->ID, "projection");

		//// pass them to the shaders 
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(renderer.objects[0].transform.getModelMatrix()));
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(renderer.objects[0].transform.getViewMatrix()));
		//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(renderer.objects[0].transform.perspectiveMat));
		//
		//renderer.render(*sjencar[0]);
		
		//glm::mat4 identity = glm::mat4(1.0f);
		// create transformations
		renderer.objects[1].transform.setPosition(2.5f, 0.0f, 0.0f);
		renderer.objects[1].transform.viewMat = viewMatrix;
		//renderer.objects[1].transform.translate(renderer.objects[1].transform.transformMat, glm::vec3(2.0f, 0.0f, 0.0f));
		renderer.objects[1].transform.rotate(renderer.objects[1].transform.transformMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		renderer.objects[1].transform.perspectiveMat = camera.getPerspectiveMatrix(glm::radians(45.0f), width, height);


		//// pass them to the shaders (3 different ways)
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(renderer.objects[1].transform.getModelMatrix()));
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(renderer.objects[1].transform.getViewMatrix()));
		//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(renderer.objects[1].transform.perspectiveMat));

		glUniform3f(eyeUniformLocation, camera.camPosition.x, camera.camPosition.y, camera.camPosition.z);
		renderer.render(*sjencar[0]);

		if (moveAlongApprox) {
			camera.camPosition = bezierCurve.approxCurvePoints[approxCounter];
			approxCounter++;
			if (approxCounter == bezierCurve.approxCurvePoints.size()) {
				approxCounter = 0;
				moveAlongApprox = false;
			}
		}

		if (moveAlongInterpol) {
			camera.camPosition = bezierCurve.interpolationCurvePoints[interpolCounter];
			interpolCounter++;
			if (interpolCounter == bezierCurve.interpolationCurvePoints.size()) {
				interpolCounter = 0;
				moveAlongInterpol = false;
			}
		}

		glUseProgram(sjencar[1]->ID);

		sjencar[1]->setUniformMat4("projection", perspectiveMatrix);
		sjencar[1]->setUniformMat4("view", viewMatrix);

		if (bezierCurve.controlPolygonPoints.size() > 0) {
			bezierCurve.drawCurves(*sjencar[1]);
		}


		glfwSwapBuffers(window);
		glfwPollEvents();

		//izlazak iz programa
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

	}


	glfwTerminate();

	return EXIT_SUCCESS;

}