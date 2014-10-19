#ifndef HEIGHTFIELDH
#define HEIGHTFIELDH

#include <stdio.h>
#include "define.h"
#include "sphere.h"


class HeightField{

public:
	int gridSize;
	float damping;
	~HeightField(void);
	HeightField(int grid, float damp, int range);
	G308_Triangle* triangles = NULL;
	G308_Point* points = NULL;
	G308_Normal* normals = NULL;
	sphere* balls = NULL;
	void update();
	void updateBalls();
	void display();
	void push(float x, float y, float r, float amnt); //pushes points within r distance from the point x,y,z downward
	void add(float x, float y, float r, float amnt);
	int collideSphere(sphere S);
	void addSphere(float x, float y, float z, float r);

private:
	int numBalls;
	void setUp();
	void calcNorms();
	void draw();
	bool inSide(float x1, float y1, float z1, float x2, float y2, float z2, float rad, bool ignoreZ);
	float** height;
	float** velocity;
	int polyList;
	int randRange;
};

#endif
