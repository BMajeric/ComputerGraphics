cmake_minimum_required(VERSION 3.5)
project(irgLab)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(irgLab/dependencies/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
option(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_EXPORTER TRUE)
set(ASSIMP_BUILD_OBJ_IMPORTER TRUE)

cmake_policy(SET CMP0175 NEW)

add_subdirectory(irgLab/dependencies/assimp)


add_subdirectory(irgLab/primjerGLM)
add_subdirectory(irgLab/primjerASSIMP)
add_subdirectory(irgLab/primjerOpenGL)


add_subdirectory(irgLab/Animation)
add_subdirectory(irgLab/BezierCurves)
add_subdirectory(irgLab/Version1)
add_subdirectory(irgLab/Version2)
add_subdirectory(irgLab/Version3)
add_subdirectory(irgLab/Version4)
add_subdirectory(irgLab/Version5)
add_subdirectory(irgLab/Version6)
add_subdirectory(irgLab/Version7_Final)

