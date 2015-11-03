#include<stdio.h>
#include<stdlib.h>

#define OFFSET 12


int num_frames;
int alg;

int loc_in_mem(int mem[], int address);
int set_dirty (int mem[], int loc    );
int is_dirty  (int mem[], int loc    );
int replace   (int mem[], int loc,    int address);
int set_R     (int mem[], int loc    );
int unset_R   (int mem[], int loc    );

int opt_alg   (int mem[], FILE* trace);
int nru_alg   (int mem[], FILE* trace);
int clock_alg (int mem[], FILE* trace);
int work_alg  (int mem[], FILE* trace);


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

int replace(int mem[], int loc, int address){
  mem[loc] = ((address>>OFFSET)<<OFFSET); 
  set_R(mem, loc);
  return 0;
}

int set_R(int mem[], int loc){
  mem[loc]|=2;  //second to last bit is 1
  return 0;
}

int unset_R(int mem[], int loc){
  mem[loc]&=0xFFFFFFFD; //second to last bit is 0
  return 0;
}
