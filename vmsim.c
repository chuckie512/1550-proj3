#include<stdio.h>
#include<stdlib.h>

#define OFFSET 12


int num_frames;
int alg;

int loc_in_mem(int mem[], int address);



int main(int argc, char ** argv){


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
