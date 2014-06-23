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
  double xPos = 1.0;//amount of voxels with positive x values
  double yPos = 1.0;//amount of voxels with positive y values
  double zPos = 1.0;//amount of voxels with positive z values
  double xNeg = 1.0;//amount of voxels with negative x values
  double yNeg = 1.0;//amount of voxels with negative y values
  double zNeg = 1.0;//amount of voxels with negative z values
  for(int i = 0;i < NUM_VOX;i++){
    if(voxels[i].x-voxels[i].size > 0)xPos++;
    if(voxels[i].y-voxels[i].size > 0)yPos++;
    if(voxels[i].z-voxels[i].size > 0)zPos++;
    if(voxels[i].x+voxels[i].size < 0)xNeg++;
    if(voxels[i].y+voxels[i].size < 0)yNeg++;
    if(voxels[i].z+voxels[i].size < 0)zNeg++;
  }
  double xSymm = abs(1-((xPos+xNeg)/NUM_VOX));
  double ySymm = abs(1-((yPos+yNeg)/NUM_VOX));
  double zSymm = abs(1-((zPos+zNeg)/NUM_VOX));
  symmetry = (xSymm+ySymm+zSymm);//try to get all symmetry values to be 1
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

double Object::distance(int one, int two){
  double x = (double)((int)voxels[one].x-(int)voxels[two].x);
  double y = (double)((int)voxels[one].y-(int)voxels[two].y);
  double z = (double)((int)voxels[one].z-(int)voxels[two].z);
  return sqrt((x*x) + (y*y) + (z*z));
}
