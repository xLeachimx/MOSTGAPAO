/*FILE: main.cpp
 *Author:Michael Huelsman
 *Abstract:
 *   Main program of the STGA
 *Notes:
 *  The commandline needs 2 arguements:
 *    1)Filename for the csv output of the program
 *    2)Filename for the scad output of the program
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

void primSoup(Object generation[], int size);//generates a random first generation

int main(int argc, char **argv){
  if(argc != 3)return 0; //Make sure that there are the required number of commandline arguments

  //setup
  unsigned int seed = time(NULL);
  srand(seed);
  Object generation[GEN_SIZE];
  Object nextGen[GEN_SIZE];

  //setup file for record keeping
  ofstream fout;
  fout.open(argv[1]);
  if(!fout){
    cout << "Problem opening record file!" << endl;
    return 0;
  }

  //record basic info
  fout << "Number of generations:" << NUM_GEN << endl;
  fout << "Generation Size:" << GEN_SIZE << endl;
  fout << "Elitism:" << ELITISM << endl;
  fout << "Number of Voxels:" << NUM_VOX << endl;
  fout << "Seed:" << seed << endl;

  //primordial soup generation
  primSoup(generation, GEN_SIZE);

  fout << "Generation,Connectivity,Phi Rating,Complexity,Symmetry" <<endl;//simple explanatory labels

  //running the GA
  for(int i = 0;i < NUM_GEN;i++){
    cout << i <<endl;

    //calcuate the fitness of all objects
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
    
    //copying generation over to the next
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

  //export best of best to scad file
  fout.open(argv[2]);
  if(!fout){
    cout << "Problem opening scad file" <<endl;
  }
  generation[0].toScad(fout);
  fout.close();
  return 0;
}


void primSoup(Object generation[], int size){
  int generator;
  voxel cloud[NUM_VOX];
  for(int i = 0;i < size;i++){
    int generator;
    for(int j = 0;j < NUM_VOX;j++){ 
      generator = rand();
      char *mod = (char *)(&(generator));
      cloud[j].x = mod[0];
      cloud[j].y = mod[1];
      cloud[j].z = mod[2];
    }
    generation[i] = Object(cloud);
  }
}
