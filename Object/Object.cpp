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
  fitness = 0;
}

Object::Object(voxel copy[NUM_VOX]){
  for(int i = 0 ;i < NUM_VOX;i++){
    voxels[i] = copy[i];
  }
  connectivity = 0.0;
  phiRating = 0.0;
  fitness = 0;
}

Object::Object(const Object &copy){
  for(int i = 0;i < NUM_VOX;i++){
    voxels[i] = copy.voxels[i];
  }
  connectivity = copy.connectivity;
  phiRating = copy.phiRating;
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
  out << connectivity << "," << phiRating << '\n';
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
  calcConnectivity();
  calcPhiRating();
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

//sets the bbox variable
void Object::calcBoundingBox(){
  bbox.xMax = voxels[0].x+voxels[0].size;
  bbox.yMax = voxels[0].y+voxels[0].size;
  bbox.zMax = voxels[0].z+voxels[0].size;
  bbox.xMin = voxels[0].x-voxels[0].size;
  bbox.yMin = voxels[0].y-voxels[0].size;
  bbox.zMin = voxels[0].z-voxels[0].size;

  for(int i = 1;i < NUM_VOX;i++){
    //reassign x
    if(bbox.xMax < voxels[i].x+voxels[i].size){
      bbox.yMax = voxels[i].x+voxels[i].size;
    }
    else if(bbox.yMin > voxels[i].x-voxels[i].size){
      bbox.yMin = voxels[i].x-voxels[i].size;
    }
    
    //reassign y
    if(bbox.xMax < voxels[i].y+voxels[i].size){
      bbox.yMax = voxels[i].y+voxels[i].size;
    }
    else if(bbox.yMin > voxels[i].y-voxels[i].size){
      bbox.yMin = voxels[i].y-voxels[i].size;
    }

    //reassign z
    if(bbox.zMax < voxels[i].z+voxels[i].size){
      bbox.zMax = voxels[i].z+voxels[i].size;
    }
    else if(bbox.zMin > voxels[i].z-voxels[i].size){
      bbox.zMin = voxels[i].z-voxels[i].size;
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

bool Object::pareToDominate(const Object &comp){
  if(connectivity <= comp.connectivity && phiRating <= comp.phiRating){
    if(connectivity < comp.connectivity || phiRating < comp.phiRating)return true;
  }
  return false;
}

double Object::distance(int one, int two){
  double x = (double)((int)voxels[one].x-(int)voxels[two].x);
  double y = (double)((int)voxels[one].y-(int)voxels[two].y);
  double z = (double)((int)voxels[one].z-(int)voxels[two].z);
  return sqrt((x*x) + (y*y) + (z*z));
}
