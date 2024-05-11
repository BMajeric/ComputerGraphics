// Local Headers
#include <Vertex.h>
#include <Transform.h>
#include <Mesh.h>
#include <Material.h>

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


class Light : public Transform {
public:
	glm::vec3 lightPos;
	glm::vec3 lightColor;

	Light(glm::vec3 lightPos, glm::vec3 lightColor);
};