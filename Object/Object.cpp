/*FILE: Object.cpp
 *Author: Michael Huelsman
 *Abstract:
 *   Implemention of the class defined in Object.hpp
 *
 */

#include "Object.hpp"
#include <cmath>
using std::sqrt;
using std::abs;
using std::endl;

Object::Object(){
  connectivity = 0.0;
  phiRating = 0.0;
  complexity = 0.0;
  symmetry = 0.0;
  fitness = 0;
}

Object::Object(voxel copy[NUM_VOX]){
  for(int i = 0 ;i < NUM_VOX;i++){
    voxels[i] = copy[i];
  }
  connectivity = 0.0;
  phiRating = 0.0;
  complexity = 0.0;
  symmetry = 0.0;
  fitness = 0;
}

Object::Object(const Object &copy){
  for(int i = 0;i < NUM_VOX;i++){
    voxels[i] = copy.voxels[i];
  }
  connectivity = copy.connectivity;
  phiRating = copy.phiRating;
  complexity = copy.complexity;
  symmetry = copy.symmetry;
  fitness = copy.fitness;
}


voxel *Object::getVoxels(){
  return voxels;
}

int Object::getConnectivity(){
  return connectivity;
}

int Object::getPhiRating(){
  return phiRating;
}

void Object::toCSV(ostream &out){
  out << connectivity << "," << phiRating << "," << complexity << "," << symmetry <<'\n';
}

void Object::toScad(ostream &out){
  out << "//Connectivity:" << connectivity << endl;
  out << "//Phi Rating:" << phiRating <<endl;
  for(int i = 0;i < NUM_VOX;i++){
    out << "translate([" << (int)voxels[i].x << "," << (int)voxels[i].y << "," << (int)voxels[i].z << "])";
    out << "sphere(r=" << (int)voxels[i].size << ");" <<endl;
  }
}

void Object::calcQuality(){
  for(int i = 0;i < NUM_VOX;i++){
    applyTransform(voxels[i]);
  }
  calcBoundingBox();
  calcConnectivity();
  calcComplexity();
  calcPhiRating();
  calcSymmetry();
}

void Object::calcFitness(Object *gen, int size){
  fitness = 0;
  for(int i = 0;i < size;i++){
    if(pareToDominate(gen[i]))fitness++;
  }
}

bool Object::operator>(const Object &comp){
  return fitness > comp.fitness;
}

bool Object::operator>=(const Object &comp){
  return fitness >= comp.fitness;
}

bool Object::operator<(const Object &comp){
  return fitness < comp.fitness;
}

bool Object::operator<=(const Object &comp){
  return fitness <= comp.fitness;
}

bool Object::operator==(const Object &comp){
  return fitness == comp.fitness;
}

bool Object::operator!=(const Object &comp){
  return fitness != comp.fitness;
}

Object &Object::operator=(const Object &copy){
  for(int i = 0 ;i < NUM_VOX;i++){
    voxels[i] = copy.voxels[i];
  }
  connectivity = copy.connectivity;
  phiRating = copy.phiRating;
  fitness = copy.fitness;
  return *this;
}

void Object::applyTransform(voxel &v){
  v.size = ((v.x | v.y) ^ v.z)|1;//ensures the size is at least 1
  v.size = (v.size < 0)?-v.size:v.size;//ensure non-negative number
}

//sets the bBox variable
void Object::calcBoundingBox(){
  bBox.xMax = voxels[0].x+voxels[0].size;
  bBox.yMax = voxels[0].y+voxels[0].size;
  bBox.zMax = voxels[0].z+voxels[0].size;
  bBox.xMin = voxels[0].x-voxels[0].size;
  bBox.yMin = voxels[0].y-voxels[0].size;
  bBox.zMin = voxels[0].z-voxels[0].size;

  for(int i = 1;i < NUM_VOX;i++){
    //reassign x
    if(bBox.xMax < voxels[i].x+voxels[i].size){
      bBox.yMax = voxels[i].x+voxels[i].size;
    }
    else if(bBox.yMin > voxels[i].x-voxels[i].size){
      bBox.yMin = voxels[i].x-voxels[i].size;
    }
    
    //reassign y
    if(bBox.xMax < voxels[i].y+voxels[i].size){
      bBox.yMax = voxels[i].y+voxels[i].size;
    }
    else if(bBox.yMin > voxels[i].y-voxels[i].size){
      bBox.yMin = voxels[i].y-voxels[i].size;
    }

    //reassign z
    if(bBox.zMax < voxels[i].z+voxels[i].size){
      bBox.zMax = voxels[i].z+voxels[i].size;
    }
    else if(bBox.zMin > voxels[i].z-voxels[i].size){
      bBox.zMin = voxels[i].z-voxels[i].size;
    }
  }
}

void Object::calcConnectivity(){
  double connections = 0.0;
  for(int i = 0;i < NUM_VOX;i++){
    for(int j = 0;j < NUM_VOX;j++){
      if(i != j){
	int comparedSize = voxels[i].size + voxels[j].size;
	if(distance(i,j) < comparedSize)connections++;
      }
    }
  }
  connectivity = abs((NUM_VOX/CON_RATIO)-(connections/NUM_VOX));
}

void Object::calcPhiRating(){
  double width = bBox.xMax-bBox.xMin;
  double height = bBox.zMax-bBox.zMin;
  double depth = bBox.yMax-bBox.yMin;
  phiRating = abs((PHI - (width/height)))+abs((PHI - (depth/width))); //actually calculate the use of golden rectangles in the bounding box
}

void Object::calcSymmetry(){
  double xPos = 0.0;//amount of voxels with positive x values
  double yPos = 0.0;//amount of voxels with positive y values
  double zPos = 0.0;//amount of voxels with psoitive z values
  for(int i = 0;i < NUM_VOX;i++){
    if(voxels[i].x > 0)xPos++;
    if(voxels[i].y > 0)yPos++;
    if(voxels[i].z > 0)zPos++;
  }
  double xSymm = abs(1-(xPos/(NUM_VOX-xPos)));
  double ySymm = abs(1-(yPos/(NUM_VOX-yPos)));
  double zSymm = abs(1-(zPos/(NUM_VOX-zPos)));
  symmetry = 3-(xSymm+ySymm+zSymm);//try to get all symmetry values to be 1
}

void Object::calcComplexity(){
  int inner = 0.0; //number of voxels inside the internal bounding box
  double xInter = (bBox.xMax-bBox.xMin)/4; 
  double yInter = (bBox.yMax-bBox.yMin)/4;
  double zInter = (bBox.zMax-bBox.zMin)/4;
  BoundingBox internal;
  internal.xMin = bBox.xMin + xInter;
  internal.xMax = bBox.xMin + (3*xInter);
  internal.yMin = bBox.yMin + yInter;
  internal.yMax = bBox.yMin + (3*yInter);
  internal.zMin = bBox.zMin + zInter;
  internal.zMax = bBox.zMin + (3*zInter);
  for(int i = 0;i < NUM_VOX;i++){
    if(voxels[i].x >= internal.xMin && voxels[i].x <= internal.xMax){
      if(voxels[i].y >= internal.yMin && voxels[i].y <= internal.yMax){
	if(voxels[i].z >= internal.zMin && voxels[i].z <= internal.zMax)inner++;
      }
    }
  }
  complexity = 2-((double)NUM_VOX/(double)inner);
}

bool Object::pareToDominate(const Object &comp){
  if(connectivity <= comp.connectivity && phiRating <= comp.phiRating && complexity <= comp.complexity && symmetry <= comp.symmetry){
    if(connectivity < comp.connectivity || phiRating < comp.phiRating || complexity < comp.complexity || symmetry < comp.symmetry)return true;
  }
  return false;
}

double Object::distance(int one, int two){
  double x = (double)((int)voxels[one].x-(int)voxels[two].x);
  double y = (double)((int)voxels[one].y-(int)voxels[two].y);
  double z = (double)((int)voxels[one].z-(int)voxels[two].z);
  return sqrt((x*x) + (y*y) + (z*z));
}
