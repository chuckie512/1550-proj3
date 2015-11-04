#include<stdio.h>
#include<stdlib.h>

#define OFFSET 12


int num_frames;
int alg;
int num_refrences;



int loc_in_mem(int mem[], int address);
int set_dirty (int mem[], int loc    );
int is_dirty  (int mem[], int loc    );
int replace   (int mem[], int loc,    int address); //TODO add more tests
int set_R     (int mem[], int loc    );
int unset_R   (int mem[], int loc    );
int get_R     (int mem[], int loc    );

int opt_alg   (int mem[], FILE * file); //TODO
int nru_alg   (int mem[], FILE * file); //TODO
int clock_alg (int mem[], FILE * file); //TODO
int work_alg  (int mem[], FILE * file); //TODO

int ff_used   (int mem[], FILE * file); //TODO




void help();

int test(int mem[]){
  int status=0;

  printf("===============================================\n");
  printf("                  TESTING                      \n");
  printf("===============================================\n");
  
  replace(mem, 0, 0xFFCDFFFD);
  
  int loc = loc_in_mem(mem, 0xFFCDFFFF);

  if(loc != 0){
    printf("TEST[0]: FAIL loc in mem is not returning the correct value, loc is %d when it should be 0\n", loc);
    status-=1;
  }
  else{
   printf("TEST[0]:  PASSED\n");
  }
  if(is_dirty(mem,0)!=0){
    printf("TEST[0.5] FAIL a clean page is being returned as %d\n",is_dirty(mem,0));
    status -=1;
  }
  else{
    printf("TEST[0.5] PASSED\n");
  }
  set_dirty(mem, 0);
  if(is_dirty(mem, 0)!=1){
    printf("TEST[1]: FAIL a page that should be dirty is being returned as clean\n");
    status -=1;
  }
  else{
   printf("TEST[1]:  PASSED\n");
  }
  
  set_R(mem,0);  
  if(get_R(mem, 0) !=1){
    printf("TEST[2]: FAIL a R bit that should be 1 is %d\n", get_R(mem,0));
    printf("mem[0]: %x\n",mem[0]);
    status -=1;
  }
  else{
    printf("TEST[2]:  PASSED\n");
  }
  
  unset_R(mem, 0);
  
  if(get_R(mem,0)!=0){
    printf("TEST[3]: FAIL a R bit that should be 0 is %d\n", get_R(mem,0));
    status-=1;
  }
  else{
    printf("TEST[3]:  PASSED\n");
  }


  printf("===============================================\n");
  printf("ERRORS FOUND: %d\n",status*-1);
  return status;
}


int main(int argc, char ** argv){
  num_frames=1;

  int mem[num_frames]; 
  int exit = test(mem);
  
  
  



  return exit;
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
  return mem[loc]&1;
}

int replace(int mem[], int loc, int address){
  mem[loc] = ((address>>OFFSET)<<OFFSET); //last 12 don't matter
  return 0;
}

int set_R(int mem[], int loc){
  mem[loc] |=2;  //second to last bit is 1
  return 0;
}

int unset_R(int mem[], int loc){
  mem[loc]&=0xFFFFFFFD; //second to last bit is 0
  return 0;
}

int get_R(int mem[], int loc){
  return (mem[loc]&2)>>1;
}

void help(){
  printf("usage: ./vmsim –n <numframes> -a <opt|clock|nru|work> [-r <refresh>] [-t <tau>] <tracefile>\n");
}


int clock_alg(int mem[], FILE *){
  int address;
  char mode;

  while(1){
    int status = fscanf(file, "%x %c", &address, &mode); //read the file
    if(status<2){  //if we didn't get everything, we're done
      break;
    }
    int loc = loc_in_mem(mem, address);
    if(loc == -1){
      //here is where we actually put it into memory, b/c it's not there
      //make sure loc is set when done
    }
    if(mode == 'w'){
      set_dirty(mem, loc);
    }
    set_R(mem, loc);
  }
}

