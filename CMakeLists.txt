cmake_minimum_required (VERSION 2.8)

# enable IDE project folders
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

project(COMP308_FINAL_PROJECT)

# OpenGL
include(FindOpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# GLFW
add_subdirectory("${PROJECT_SOURCE_DIR}/external/glfw-3.0.4" "${PROJECT_BINARY_DIR}/external/glfw")
include_directories("${PROJECT_SOURCE_DIR}/external/glfw-3.0.4/include" )

# GLEW
add_subdirectory("${PROJECT_SOURCE_DIR}/external/glew-1.11.0" "${PROJECT_BINARY_DIR}/external/glew")
include_directories("${PROJECT_SOURCE_DIR}/external/glew-1.11.0/include")

# GLM
include_directories("${PROJECT_SOURCE_DIR}/external/glm")

# compiler flags 
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# Force to always compile with W4
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp /arch:SSE2")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oi")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Werror=return-type -pedantic -std=c++11 -g -msse2 -fopenmp")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Werror=return-type -pedantic -std=c++11 -g -msse2 -fopenmp")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

# output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")

# source
add_subdirectory(src)

# set IDE display folders
SET_PROPERTY(TARGET glfw PROPERTY FOLDER "ext")
SET_PROPERTY(TARGET glew PROPERTY FOLDER "ext")
