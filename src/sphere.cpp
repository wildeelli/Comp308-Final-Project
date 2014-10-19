#include <stdio.h>
#include "sphere.h"

sphere::sphere(float xin,float yin,float zin,float rin){
	x = xin; y = yin; z = zin; r = rin;

}

void sphere::update(){
	z -= 0.3;
}

void sphere::bump(){
	z += 0.25;
}
