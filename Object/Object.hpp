/*FILE: Object.hpp
 *Author: Michael Huelsman
 *Abstract:
 *   A genetic object that expresses its genetic information in 3D voxels 
 *
 */

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <ostream>
using std::ostream;

struct voxel{
  char x;
  char y;
  char z;
  char size;
};

const int NUM_VOX = 300;
const int CON_RATIO = 4; //Ratio for maximum connectivity
const int PHI = 1.618033988749894848204586834365;

class Object{
public:
  //constructors
  Object();
  Object(voxel copy[NUM_VOX]);
  Object(const Object &copy);

  //accessors
  voxel *getVoxels();
  int getConnectivity();
  int getPhiRating();
  void toCSV(ostream &out);//outputs quality values

  void calcQuality();

  //conparative operators (they compare using pareto dominance over the connectivity and phiRating qualities)
  bool operator>(const Object &comp);
  bool operator>=(const Object &comp);
  bool operator<(const Object &comp);
  bool operator<=(const Object &comp);
  bool operator==(const Object &comp);
  bool operator!=(const Object &comp);

  //simple assignment operator
  Object &operator=(const Object &copy);
protected:
  void applyTransform(voxel &v);//This is where the schema trasform happens
  void calcConnectivity();
  void calcPhiRating();
  bool pareToDominate(const Object &comp);//is this object pareto dominant over the comp object
  bool pareToEqual(const Object &comp);//is the object pareto equivalent with the comp object
  double distance(int one,int two);//finds the distance between two voxels
private:
  voxel voxels[NUM_VOX];
  double connectivity;
  double phiRating;
};

#endif
