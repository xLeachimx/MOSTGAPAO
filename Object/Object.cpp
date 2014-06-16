/*FILE: Object.cpp
 *Author: Michael Huelsman
 *Abstract:
 *   Implemention of the class defined in Object.hpp
 *
 */

#include "Object.hpp"
#include <cmath>
#include <iostream>
using std::sqrt;
using std::abs;
using std::cout;
using std::endl;

Object::Object(){
  connectivity = 0.0;
  phiRating = 0.0;
  fitness = 0;
}

Object::Object(voxel copy[NUM_VOX]){
  for(int i = 0 ;i < NUM_VOX;i++){
    voxels[i] = copy[i];
    applyTransform(voxels[i]);
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
  out << connectivity << "," << phiRating << "," << fitness <<'\n';
}

void Object::calcQuality(){
  calcConnectivity();
  calcPhiRating();
}

void Object::calcFitness(Object *gen, int size, int skip){
  fitness = 0;
  for(int i = 0;i < size;i++){
    if(i == skip)continue;
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
    applyTransform(voxels[i]);
  }
  connectivity = copy.connectivity;
  phiRating = copy.phiRating;
  fitness = copy.fitness;
  return *this;
}

void Object::applyTransform(voxel &v){
  v.size = ((v.x + v.y + v.z)/3)|1;//ensures the size is at least 1
  v.size = (v.size < 0)?-v.size:v.size;
}

void Object::calcConnectivity(){
  double connections = 0.0;
  for(int i = 0;i < NUM_VOX;i++){
    for(int j = 0;j < NUM_VOX;j++){
      if(i != j){
	applyTransform(voxels[i]);
	applyTransform(voxels[j]);
	int comparedSize = voxels[i].size + voxels[j].size;
	if(distance(i,j) < comparedSize)connections++;
      }
    }
  }
  connectivity = abs((NUM_VOX/CON_RATIO)-(connections/NUM_VOX));
}

void Object::calcPhiRating(){
  double maxX = voxels[0].x+voxels[0].size;
  double maxY = voxels[0].y+voxels[0].size;
  double maxZ = voxels[0].z+voxels[0].size;
  double minX = voxels[0].x-voxels[0].size;
  double minY = voxels[0].y-voxels[0].size;
  double minZ = voxels[0].z-voxels[0].size;

  for(int i = 1;i < NUM_VOX;i++){
    //reassign x
    if(maxX < voxels[i].x+voxels[i].size){
      maxX = voxels[i].x+voxels[i].size;
    }
    else if(minX > voxels[i].x-voxels[i].size){
      minX = voxels[i].x-voxels[i].size;
    }
    
    //reassign y
    if(maxY < voxels[i].y+voxels[i].size){
      maxY = voxels[i].y+voxels[i].size;
    }
    else if(minY > voxels[i].y-voxels[i].size){
      minY = voxels[i].y-voxels[i].size;
    }

    //reassign z
    if(maxZ < voxels[i].z+voxels[i].size){
      maxZ = voxels[i].z+voxels[i].size;
    }
    else if(minZ > voxels[i].z-voxels[i].size){
      minZ = voxels[i].z-voxels[i].size;
    }
  }
  double width = maxX-minX;
  double height = maxZ-minZ;
  double depth = maxY-minY;
  if(width <= 0)cout << "No width" <<endl;
  if(height <= 0)cout << "No height" <<endl;
  if(depth <= 0)cout << "No depth" <<endl;
  phiRating = abs((PHI - (width/height))+(PHI - (depth/width)));
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
