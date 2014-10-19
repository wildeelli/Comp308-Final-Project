#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <GL/glut.h>
#include "HeightField.h"
#include "define.h"


HeightField::~HeightField(){
	delete points;
	delete triangles;
	delete normals;
	delete balls;
}

HeightField::HeightField(int grid, float damp, int range){
	setvbuf(stdout, NULL, _IONBF, 0);
	gridSize = grid;
	damping = damp;
	randRange = range;
	numBalls = 0;

	//allocate arrays of velocities
	velocity = (float**) calloc(gridSize,sizeof(float*));
	for(int i = 0;i<gridSize;i++){
		velocity[i] = (float*) calloc(gridSize,sizeof(float));
	}
	//allocate array of heights
	height = (float**) calloc(gridSize,sizeof(float*));
	for(int i = 0;i<gridSize;i++){
		height[i] = (float*) calloc(gridSize,sizeof(float));
	}
	if(range > 0){
		for(int i = 0;i<gridSize;i++){
			for(int j=0;j<gridSize;j++){
				height[i][j] = rand() %randRange;
			}
		}
	}
	if(points){
		delete points;
	}
	//printf("Allocated point array\n");
	points = new G308_Point[gridSize*gridSize];
	if(triangles){
			delete triangles;
		}
	//printf("allocated triangle array\n");
	triangles = new G308_Triangle[2*(gridSize-1)*(gridSize-1)];
	if(normals){
				delete normals;
			}
	//printf("allocated Normal Array\n");
	normals = new G308_Normal[gridSize*gridSize];
	//normals = new G308_Normal[2*(gridSize-1)*(gridSize-1)];
	if(balls){
		delete balls;
	}
	//printf("allocated Normal Array\n");
	balls = new sphere[10];

	setUp();
}

void HeightField::setUp(){
	int index = 0; //gridSize*gridSize;
	for(int i = 0; i < gridSize; i++){
		for(int j = 0; j < gridSize; j++){
			points[index].x = i;
			points[index].y = j;
			points[index].z = height[i][j];
			//printf("Point %i = %i,%i,%f \n",index,i,j,height[i][j]);
			index++;
		}
	}
	int skip = 0;
	int num = 0;
	for(index = 0; index < (2*(gridSize-1)*(gridSize-1)); index++){
			triangles[index].v1 = (skip*gridSize) + num;		//1
			triangles[index].v2 = (skip*gridSize) + num + 1;	//2
			triangles[index].v3 = ((skip+1)*gridSize) + num;	//3
			//printf("Triangle %i = points: %i,%i,%i \n",index,triangles[index].v1,triangles[index].v2,triangles[index].v3);
			index++;
			triangles[index].v1 = (skip*gridSize) + num + 1; 	//2
			triangles[index].v2 = ((skip+1)*gridSize) + num +1;	//4
			triangles[index].v3 = ((skip+1)*gridSize) + num;	//3
			//printf("Triangle %i = points: %i,%i,%i \n",index,triangles[index].v1,triangles[index].v2,triangles[index].v3);
			//index++;
			num++;
			if(num == (gridSize-1)){
				num = 0;
				skip++;
			}
	}
}

void HeightField::calcNorms(){
	int index = 0;
	for(int i = 0;i < gridSize;i++){
		for(int j = 0;j < gridSize;j++){
			float A;
			float B;
			float C;
			float D;
			if(i == 0){
				A = height[i][j];
			}
			else{
				A = height[i-1][j];
			}
			if(i+1 == gridSize){
				B = height[gridSize-1][j];
			}
			else{
				B = height[i+1][j];
			}
			if(j == 0){
				C = height[i][j];
			}
			else{
				C = height[i][j-1];
			}
			if(j+1 == gridSize){
				D = height[gridSize-1][j];
			}
			else{
				D = height[i][j+1];
			}

			normals[index].x = A - B;
			normals[index].y = C - D;
			normals[index].z = 2;

			float length = sqrt((normals[index].x*normals[index].x)+(normals[index].y*normals[index].y)+(normals[index].z*normals[index].z));

			normals[index].x /= length;
			normals[index].y /= length;
			normals[index].z /= length;

			//printf("Normal for point %i = %f,%f,%f\n",index,normals[index].x,normals[index].y,normals[index].z);
			index++;
		}
	}
}


void HeightField::push(float x,float y,float r, float amnt){
	for(int i = 0; i < gridSize; i++){
		for(int j = 0; j<gridSize; j++){
			if(inSide(i,j,height[i][j],x,y,0,r,true)){
				velocity[i][j] -= amnt;
			}
			//printf("Point %i, %i height = %f \n",i,j,height[i][j]);
		}
	}
}

void HeightField::add(float x,float y,float r,float amnt){
	for(int i = 0; i < gridSize; i++){
			for(int j = 0; j<gridSize; j++){
				if(inSide(i,j,height[i][j],x,y,0,r,true)){
					height[i][j] += amnt;
				}
				//printf("Point %i, %i height = %f \n",i,j,height[i][j]);
			}
		}
}

int HeightField::collideSphere(sphere s){
	int moved = 0;
	for(int i = 0; i< gridSize; i++){
		for(int j = 0; j<gridSize; j++){
			if(inSide(s.x,s.y,s.z,i,j,height[i][j],s.r,false)){
				moved++;
				velocity[i][j] += s.v;
			}
		}
	}
	return moved;
}

bool HeightField::inSide(float x1, float y1, float z1, float x2, float y2, float z2, float rad, bool ignoreZ){
	if(ignoreZ) return rad > sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
	return rad > sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2));
}

void HeightField::display(){
//	glShadeModel(GL_SMOOTH);
//	draw();
//	glCallList(polyList);
//	if(numBalls > 0){
//		for(int i = 0; i < numBalls; i++){
//			glTranslated(balls[i].x,balls[i].y,balls[i].z);
////			glutSolidSphere(balls[i].r,10,10);
//			glTranslated(-balls[i].x,-balls[i].y,-balls[i].z);
//		}
//	}
}

void HeightField::draw() {
	//printf("\nCreating PolyGeometry!\n");;
//	if (polyList != 0){
//		glDeleteLists(polyList, 1);
//	}
	// Assign a display list; return 0 if err
//	polyList = glGenLists(1);
//	glNewList(polyList, GL_COMPILE);
//		//add each face to display list.
//		int i = 0;
//		glShadeModel(GL_SMOOTH);
//		while(i < 2*(gridSize-1)*(gridSize-1)){
//		glBegin(GL_POLYGON);
//			//First point and its normal
//			glNormal3f(normals[triangles[i].v1].x,normals[triangles[i].v1].y,normals[triangles[i].v1].z);
//			glVertex3f(points[triangles[i].v1].x,points[triangles[i].v1].y,points[triangles[i].v1].z);
//
//			//Second point and its normal
//			glNormal3f(normals[triangles[i].v2].x,normals[triangles[i].v2].y,normals[triangles[i].v2].z);
//			glVertex3f(points[triangles[i].v2].x,points[triangles[i].v2].y,points[triangles[i].v2].z);
//
//			//Third point and its normal
//			glNormal3f(normals[triangles[i].v3].x,normals[triangles[i].v3].y,normals[triangles[i].v3].z);
//			glVertex3f(points[triangles[i].v3].x,points[triangles[i].v3].y,points[triangles[i].v3].z);
//			i++;
//		glEnd();
//		}
//	glEndList();
}

void HeightField::update(){
	updateBalls();
	int num = gridSize*gridSize;
	for(int i = 0; i < gridSize; i++){
			for(int j = 0; j<gridSize; j++){
				int iDown = i-1; int iUp = i+1;
				int jDown = j-1; int jUp = j+1;

				if(iDown == -1){ iDown = 0;}
				if(jDown == -1){ jDown = 0;}

				if(iUp == gridSize){ iUp = gridSize-1;}
				if(jUp == gridSize){ jUp = gridSize-1;}

				velocity[i][j] += (height[iDown][j] + height[iUp][j] + height[i][jDown] + height[i][jUp])/4 - height[i][j];
				velocity[i][j] *= damping;
				//printf("Point %i, %i velocity = %f \n",i,j,velocity[i][j]);
			}
		}
	num = 0;
	for(int i = 0; i < gridSize; i++){
			for(int j = 0; j<gridSize; j++){
				height[i][j] += velocity[i][j];
				points[num++].z = height[i][j];
				//printf("Point %i, %i height = %f \n",i,j,height[i][j]);
			}
	}

	calcNorms();
}

void HeightField::addSphere(float x, float y, float z, float r){
	if(numBalls < 10){
		printf("Created ball %f,%f,%f radius %f \n",x,y,z,r);
		balls[numBalls].x = x;
		balls[numBalls].y = y;
		balls[numBalls].z = z;
		balls[numBalls].r = r;
		numBalls++;
	}
}

void HeightField::updateBalls(){
	for(int i = 0; i < numBalls; i++){
		balls[i].v -= 0.05;
		balls[i].z += balls[i].v;
		//printf("Ball at %f,%f now at height = %f \n", balls[i].x, balls[i].y, balls[i].z);
		int colls = collideSphere(balls[i]);
		if(colls>0){
			printf("%i Collisions", colls);
			balls[i].v += colls / balls[i].r * 0.01;
		}
	}
}


