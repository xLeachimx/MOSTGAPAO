/*FILE: Object.cpp
 *Author: Michael Huelsman
 *Abstract:
 *   Implemention of the class defined in Object.hpp
 *
 */

#include "Object.hpp"
#include <cmath>
using std::sqrt;

Object::Object(){
  connectivity = 0.0;
  phiRating = 0.0;
}

Object::Object(voxel copy[VOX_NUM]){
  for(int i = 0 ;i < NUM_VOX;i++){
    voxels[i] = copy[i];
    applyTransform(voxels[i]);
  }
}

Object::Object(const Object &copy){
  for(int i = 0;i < NUM_VOX;i++){
    voxels[i] = copy.voxels[i];
  }
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

void Object::calcQuality(){
  calcConnectivity();
  calcPhiRating();
}


bool Object::operator>(const Object &comp){
  return pareToDominate(comp);
}

bool Object::operator>=(const Object &comp){
  return pareToDominate(comp) || pareToEqual(comp);
}

bool Object::operator<(const Object &comp){
  return !(pareToDominate(comp) || pareToEqual(comp));
}

bool Object::operator<=(const Object &comp){
  return pareToDominate(comp);
}

bool Object::operator==(const Object &comp){
  return pareToEqual(comp);
}

bool Object::operator!=(const Object &comp){
  return !pareToEqual(comp);
}

Object &Object::operator=(const Object &copy){
  for(int i = 0 ;i < NUM_VOX;i++){
    voxels[i] = copy[i];
    applyTransform(voxels[i]);
  }
  return *this;
}

void Object::applyTansform(voxel &v){
  v.size = (v.x | v.y) ^ v.z;
}

void Object::calcConnectivity(){
  double connections = 0.0;
  for(int i = 0;i < NUM_VOX;i++){
    for(int j = 0;j < NUM_VOX;j++){
      if(i != j){
	int comparedSize = voxel[i].size + voxel[j].size;
	if(distance(i,j) < comparedSize)connections++;
      }
    }
  }
  connectivity = connections/NUM_VOX;
}

void Object::calcPhiRating(){
  double maxX = voxel[0].x+voxel.size;
  double maxY = voxel[0].x+voxel.size;
  double maxZ = voxel[0].x+voxel.size;
  double minX = voxel[0].x-voxel.size;
  double minY = voxel[0].x-voxel.size;
  double minZ = voxel[0].x-voxel.size;

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
}

bool Object::pareToDominate(const Object &comp){
  if(connectivity <= comp.connectivity && phiRating <= comp.phiRating){
    if(connectivty < comp.connectivity || phiRating < comp.phiRating)return true;
  }
  return false;
}

bool Object::pareToEqual(const Object &comp){
  return connectivity == comp.connectivity && phiRating == comp.phiRating);
}

double Object::distance(int one, int two){
  double x = (double)((int)voxels[one].x-(int)voxel[two].x);
  double y = (double)((int)voxels[one].y-(int)voxel[two].y);
  double z = (double)((int)voxels[one].z-(int)voxel[two].z);
  return sqrt((x*x) + (y*y) + (z*z));
}
