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
  out << "//Complexity:" << complexity <<endl;
  out << "//Symmetry:" << symmetry <<endl;
  out << "hull(){" << endl;
  for(int i = 0;i < NUM_VOX;i++){
    out << "translate([" << (int)voxels[i].x << "," << (int)voxels[i].y << "," << (int)voxels[i].z << "])";
    out << "sphere(r=" << (int)voxels[i].size << ");" <<endl;
  }
  out << "}" <<endl;
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
	if(distance(voxels[i],voxels[j]) < comparedSize)connections++;
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

//tests for symmetry accross the planes: x=0, y=0, and z=0
void Object::calcSymmetry(){
  symmetry = 0.0;
  for(int i = 0;i < NUM_VOX;i++){
    voxel antiX, antiY, antiZ;
    antiX = antiY = antiZ = voxels[i];
    antiX.x = -antiX.x;
    antiY.y = -antiY.y;
    antiZ.z = -antiZ.z;
    voxel xClosest = voxels[0];
    double xDist = distance(xClosest,antiX);
    voxel yClosest = voxels[0];
    double yDist = distance(xClosest,antiY);
    voxel zClosest = voxels[0];
    double zDist = distance(xClosest,antiZ);
    for(int j = 1; j < NUM_VOX;j++){
      //closest object to hypothetical reflection on xAxis
      if(xDist > distance(antiX,voxels[j])){
	xClosest = voxels[j];
	xDist = distance(antiX,voxels[j]);
      }

      if(yDist > distance(antiY,voxels[j])){
	yClosest = voxels[j];
	yDist = distance(antiY,voxels[j]);
      }

      if(zDist > distance(antiZ,voxels[j])){
	zClosest = voxels[j];
	zDist = distance(antiZ,voxels[j]);
      }
    }
    //add to the symmetry count
    double sizeComp = abs(xClosest.size-antiX.size) + abs(yClosest.size-antiY.size) + abs(zClosest.size-antiZ.size);
    symmetry += (xDist+yDist+zDist)+sizeComp;
  }
}

void Object::calcComplexity(){
  int outer = 0.0; //number of voxels inside the internal bounding box
  double xInter = (bBox.xMax-bBox.xMin)/4.0; 
  double yInter = (bBox.yMax-bBox.yMin)/4.0;
  double zInter = (bBox.zMax-bBox.zMin)/4.0;
  BoundingBox internal;
  internal.xMin = bBox.xMin + xInter;
  internal.xMax = bBox.xMin + (3*xInter);
  internal.yMin = bBox.yMin + yInter;
  internal.yMax = bBox.yMin + (3*yInter);
  internal.zMin = bBox.zMin + zInter;
  internal.zMax = bBox.zMin + (3*zInter);
  outer = NUM_VOX;
  for(int i = 0;i < NUM_VOX;i++){
    if(voxels[i].x-voxels[i].size >= internal.xMin && voxels[i].x+voxels[i].size <= internal.xMax){
      if(voxels[i].y-voxels[i].size >= internal.yMin && voxels[i].y+voxels[i].size <= internal.yMax){
	if(voxels[i].z-voxels[i].size >= internal.zMin && voxels[i].z+voxels[i].size <= internal.zMax)outer--;
      }
    }
  }
  complexity = abs(5-(((double)NUM_VOX-outer)/((double)outer)));
}

bool Object::pareToDominate(const Object &comp){
  if(connectivity <= comp.connectivity && phiRating <= comp.phiRating && complexity <= comp.complexity && symmetry <= comp.symmetry){
    if(connectivity < comp.connectivity || phiRating < comp.phiRating || complexity < comp.complexity || symmetry < comp.symmetry)return true;
  }
  return false;
}

double Object::distance(voxel &one, voxel &two){
  double x = (double)((int)one.x-(int)two.x);
  double y = (double)((int)one.y-(int)two.y);
  double z = (double)((int)one.z-(int)two.z);
  return sqrt((x*x) + (y*y) + (z*z));
}
