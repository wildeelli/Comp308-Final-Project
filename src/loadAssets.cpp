/*
 * loadAssets.cpp
 *
 *  Created on: 4/10/2014
 *      Author: Elliot Wilde
 */

#include <glm/glm.hpp>
#include <vector>
#include <stdio.h>
#include "loadAssets.hpp"
//#include <string>
#include <algorithm>
#include <iostream>
//#include
#include <fstream>
#include <sstream>
#include <string.h>

using namespace std;
using namespace glm;

bool readOBJ(const char* path, std::vector<vec3> & vertices, std::vector<vec3> & normals, std::vector<vec2> & uvs){
	vector< uint > vertexIndices, uvIndices, normalIndices;
	vector<vec3> v;
	vector<vec3> n;
	vector<vec2> t;
	if (path==NULL){
		cout << "Blank File path, aborting" << endl;
		return false;
	}
	ifstream file (path, ifstream::in);
	while (file.good()){
		string buf;
		getline(file, buf);

		istringstream line (buf);
		string token;
		line >> token;
		if (line.fail())
			continue;
		if (token[0]=='#') // is the line a comment
			continue;
		if (strcmp(token.c_str(), "v")==0){
			vec3 vert;
			line >> vert.x >> vert.y >> vert.z;
			v.push_back(vert);
		} else if (strcmp(token.c_str(), "vn")==0){
			vec3 norm;
			line >> norm.x >> norm.y >> norm.z;
			n.push_back(norm);
		} else if (strcmp(token.c_str(), "vt")==0){
			vec2 text;
			line >> text.s >> text.t;
			t.push_back(text);
		} else if (strcmp(token.c_str(), "f")==0){
			string v1, v2, v3;
			uint vind[3], texind[3], normind[3];
			// and now i use scanf
			line >> v1 >> v2 >> v3;
			string l = v1+v2+v3;
			int count = std::count(l.begin(), l.end(), '/');
			if (count == 6){
				string t;
				stringstream s1 (v1);
				getline(s1, t, '/');
				vind[0] = atoi(t.c_str());
				getline(s1, t, '/');
				texind[0] = atoi(t.c_str());
				getline(s1, t, '/');
				normind[0] = atoi(t.c_str());

				stringstream s2 (v2);
				getline(s2, t, '/');
				vind[1] = atoi(t.c_str());
				getline(s2, t, '/');
				texind[1] = atoi(t.c_str());
				getline(s2, t, '/');
				normind[1] = atoi(t.c_str());

				stringstream s3 (v3);
				getline(s3, t, '/');
				vind[2] = atoi(t.c_str());
				getline(s3, t, '/');
				texind[2] = atoi(t.c_str());
				getline(s3, t, '/');
				normind[2] = atoi(t.c_str());

				vertexIndices.push_back(vind[0]);
				vertexIndices.push_back(vind[1]);
				vertexIndices.push_back(vind[2]);
				uvIndices.push_back(texind[0]);
				uvIndices.push_back(texind[1]);
				uvIndices.push_back(texind[2]);
				normalIndices.push_back(normind[0]);
				normalIndices.push_back(normind[1]);
				normalIndices.push_back(normind[2]);
			} else if (count==0){
				vertexIndices.push_back(atoi(v1.c_str()));
				vertexIndices.push_back(atoi(v2.c_str()));
				vertexIndices.push_back(atoi(v3.c_str()));

			} else if (count==3){

			}
		}


	}
		for (int i=0; i<vertexIndices.size(); ++i){
			uint vertindex = vertexIndices[i];
			vec3 vertex = v[vertindex-1];
			vertices.push_back(vertex);

			if (!t.empty()){
				uint uvindex = uvIndices[i];
				if (uvindex){
					vec2 uv = t[uvindex-1];
					uvs.push_back(uv);
				}
			}

			if (!n.empty()){
				uint normindex = normalIndices[i];
				if (normindex){
					vec3 normal = n[normindex-1];
					normals.push_back(normal);
				}
			}
		}
	cout << "Vertices: " << v.size() << endl;
	cout << "Normals: " << n.size() << endl;
	cout << "UV Coords: " << t.size() << endl;
	return false;
}


