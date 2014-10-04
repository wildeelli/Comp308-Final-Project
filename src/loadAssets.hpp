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

using namespace std;
using namespace glm;

bool readOBJ(const char* path, vector<vec3> & vertices, vector<vec3> & normals, vector<vec2> & uvs);



#endif /* LOADASSETS_HPP_ */
