/*
 * loadAssets.hpp
 *
 *  Created on: 4/10/2014
 *      Author: Bandit
 */

#ifndef LOADASSETS_HPP_
#define LOADASSETS_HPP_

#include <glm/glm.hpp>
#include <vector>

//using namespace std;
//using namespace glm;

bool readOBJ(const char* path, std::vector<glm::vec3> & vertices, std::vector<glm::vec3> & normals, std::vector<glm::vec2> & uvs);
bool readOBJ(const char* path, std::vector<glm::vec3> & vertices, std::vector<glm::vec3> & normals, std::vector<glm::vec2> & uvs, std::vector<glm::vec3> & pickcolours);




#endif /* LOADASSETS_HPP_ */
