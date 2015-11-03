#include<stdio.h>
#include<stdlib.h>

#define OFFSET 12


int num_frames;
int alg;

int loc_in_mem(int mem[], int address);
int set_dirty (int mem[], int loc    );
int is_dirty  (int mem[], int loc    );

int main(int argc, char ** argv){
  num_frames=1;

  int mem[num_frames]; 
  
  
  
  



  return 0;
}

/**
 * returns index if found, -1 otherwise
 *
 */
int loc_in_mem(int mem[], int address){

  int found = -1;
  int i;
  address= address >>OFFSET;  //shift away the offset, we don't want it
  for(i=0; i<num_frames; i++){
    if(address = mem[i] >>OFFSET){
      found = i;
      break;
    }
  }
  return found;


}

int set_dirty(int mem[], int loc){
  mem[loc] |=1;
  return 0;
}

int is_dirty(int mem[], int loc){
  return mem[loc]&=1;
}
