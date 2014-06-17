/*FILE: main.cpp
 *Author:Michael Huelsman
 *Abstract:
 *   Main program of the STGA
 */
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
using std::cout;
using std::endl;
using std::ofstream;
using std::srand;
using std::time;

#include "Object.hpp"
#include "Genetics.hpp"

const int NUM_GEN = 500;
const int GEN_SIZE = 100;
const int ELITISM = 2;

int main(int argc, char **argv){
  if(argc != 3)return 0;
  //setup
  unsigned int seed = time(NULL);
  srand(seed);
  Object generation[GEN_SIZE];
  voxel cloud[NUM_VOX];
  //setup file for record keeping
  ofstream fout;
  fout.open(argv[1]);
  if(!fout){
    cout << "Problem opening record file!" <<endl;
    return 0;
  }
  //record basic info
  fout << "Number of generations:" << NUM_GEN <<endl;
  fout << "Generation Size:" << GEN_SIZE<<endl;
  fout << "Elitism:" << ELITISM <<endl;
  fout << "Number of Voxels:" << NUM_VOX <<endl;
  //primordial soup generator
  for(int i = 0;i < GEN_SIZE;i++){
    int generator;
    for(int j = 0;j < NUM_VOX;j++){ 
      generator = rand();
      char *mod = (char *)(&(generator));
      cloud[j].x = mod[1];
      cloud[j].y = mod[2];
      cloud[j].z = mod[3];
      cloud[j].size = 1;
    }
    generation[i] = Object(cloud);
  }
  fout << "Generation,Top Connectivity,Top PhiRating" <<endl;
  //running the GA
  Object nextGen[GEN_SIZE];
  for(int i = 0;i < NUM_GEN;i++){
    cout << i <<endl;
    for(int j = 0;j < GEN_SIZE;j++){
      generation[j].calcQuality();
    }
    for(int j = 0;j < GEN_SIZE;j++){
      generation[j].calcFitness(generation, GEN_SIZE);
    }
    genSort(generation,GEN_SIZE);//sort according to dominance
    //record keeping
    fout << i << ",";
    generation[0].toCSV(fout);
    //elitism
    for(int j = 0;j < ELITISM;j++){
      nextGen[j] = generation[j];
    }
    //reproduction
    Children product;
    for(int j = 0;j < ((GEN_SIZE-ELITISM)/2);j++){
      product = crossover(selection(generation,GEN_SIZE));
      nextGen[ELITISM+(2*j)] = product.first;
      nextGen[ELITISM+(2*j)+1] = product.second;
    }
    //copying generation over
    for(int j = 0;j < GEN_SIZE;j++){
      generation[j] = nextGen[j];
    }
  }
  //post-run records
  for(int j = 0;j < GEN_SIZE;j++){
    generation[j].calcQuality();
  }
  for(int j = 0;j < GEN_SIZE;j++){
    generation[j].calcFitness(generation, GEN_SIZE);
  }
  genSort(generation,GEN_SIZE);//sort according to dominance
  //record keeping
  fout << NUM_GEN << ",";
  generation[0].toCSV(fout);
  fout.close();
  fout.open(argv[2]);
  if(!fout){
    cout << "Problem opening scad file" <<endl;
  }
  generation[0].toScad(fout);
  fout.close();
  return 0;
}
