/*FILE: Genetics.cpp
 *Author: Michael Huelsman
 *Abstract:
 *   Implementation of algortihms in Genetics.hpp 
 */

#include <cstdlib>
using namespace std::rand;

//needed subfuntions
void randomize(Object gen[], int size, int cycles);
void swap(Object &one, Object &two);

Children crossover(Parents p){
  int crossed = 0;
  bool father = true;
  voxel first[NUM_VOX];
  voxel second[NUM_VOX];
  for(int i = 0;i < NUM_VOX;i++){
    if(rand()%100 < CROSS_PER && crossed < CROSS_POINTS){
      father = !father;
      crossed++;
    }
    first[i] = father?p.father.getVoxels[i]:p.mother.getVoxels[i];
    second[i] = father?p.mother.getVoxels[i]:p.father.getVoxels[i];
  }
  mutate(first,NUM_VOX);
  mutate(second,NUM_VOX);
  Children temp;
  temp.first = Object(first);
  temp.second = Object(second);
}

void mutate(voxel v[], int size){
  for(int i = 0;i < size;i++){
    if(rand()%100 < MUTATION_PER){
      int *mutation = new int;
      *mutation = rand();
      v[i].x = ((char *)mutation)[1];
      v[i].y = ((char *)mutation)[2];
      v[i].z = ((char *)mutation)[3];
    }
  }
}

Parents selection(Object gen[], int size){
  randomize(gen,size*2);
  genSort(gen,SELECTION_SIZE);
  Parents result;
  result.father = gen[0];
  result.mother = gen[1];
  return result;
}

void genSort(Object *gen, int size){
  if(size <= 1)return;
  if(size == 2){
    if(gen[0] > gen[1]){
      swap(gen[0],gen[1]);
    }
    return;
  }
  Object *pivot = gen[0];
  int left = 1;
  int right = size - 1;
  while(left < right){
    if(gen[left] > *pivot){
      if(gen[right] < *pivot){
	swap(gen[left],gen[right]);
	left++;
	right--;
      }
      else{
	right--;
      }
    }
    else{
      left++;
    }
  }
  swap(gen[0],gen[right]);
  genSort(gen,right);
  genSort(&gen[left],size-left);
}

void randomize(Object gen[], int size, int cycles){
  for(int i = 0;i < cycles;i++){
    int one = rand()%size;
    int two = rand()%size;
    Object temp = gen[one];
    gen[one] = gen[two];
    gen[two] = temp;
  }
}


void swap(Object &one, Object &two){
  Object temp = one;
  one = two;
  two = temp;
}
