#include<stdio.h>
#include<stdlib.h>

#define OFFSET 12


int num_frames;
int alg;



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

void print_results(long accesses, long faults, long writes);

void help();
int test(int mem[]);

struct llnode{
struct llnode * next;
long val;
};

int add_llnode(struct llnode * root, int new_val);
int find_val_after(struct llnode * root, int cur_val);

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


  struct llnode *test_root = malloc( sizeof(struct llnode) );
  test_root->next = NULL;
  test_root->val  = 0;
  
  add_llnode(test_root, 5  );
  add_llnode(test_root, 6  );
  add_llnode(test_root, 20 );
  add_llnode(test_root, 200); 

  if(find_val_after(test_root, 0) != 5){
    printf("TEST[4]: FAIL the next val should be 5 but instead is %d\n", find_val_after(test_root, 0) );
    status-=1;
  }
  else{
    printf("TEST[4]:  PASSED\n");
  }

  if(find_val_after(test_root, 5) !=6 ){
    printf("TEST[5]: FAIL the next val should be 6 but instead is %d\n", find_val_after(test_root, 5) );
    status-=1;
  }
  else{
    printf("TEST[5]:  PASSED\n");
  }



  printf("===============================================\n");
  printf("ERRORS FOUND: %d\n",status*-1);
  return status;
}


int main(int argc, char ** argv){
  num_frames=8;

  FILE* file = fopen("./gcc.trace", "r");
  int mem[num_frames]; 
  int i;
  for(i=0; i<num_frames; i++)
    mem[i]=0;
  //int exit = clock_alg(mem,file);
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
    if(address == mem[i] >>OFFSET){
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


int clock_alg(int mem[], FILE * file){
  int address;
  char mode;

  long faults   = 0;
  long accesses = 0;
  long writes   = 0;

  int clock_hand=0;

  while(1){
    int status = fscanf(file, "%x %c", &address, &mode); //read the file
    if(status<2){  //if we didn't get everything, we're done
      break;
    }

    accesses++;
    
    int loc = loc_in_mem(mem, address);
    if(loc == -1){
      //here is where we actually put it into memory, b/c it's not there
      faults++;
      printf("page fault - ");
      //TODO log a no-evict fault as no evict not clean
      //find the evicatable page
      int R = get_R(mem, clock_hand);
      while(R==1){
        unset_R(mem, clock_hand);
        clock_hand++;
        clock_hand%=num_frames;
        R = get_R(mem, clock_hand);
      }
      //clock_hand now points to an evictable page
      
      
      if(is_dirty(mem, clock_hand)==1){//we have to write to disk
        printf("evict dirty\n");
        writes++;
      }
      else{ //it's clean
        printf("evict clean\n");
      }

      replace(mem, clock_hand, address);
     
      loc=clock_hand;
      clock_hand++;
      clock_hand%=num_frames;
    }
    else{
      printf("hit\n");
    }
    if(mode == 'w'||mode=='W'){
      set_dirty(mem, loc);
    }
    set_R(mem, loc);
  }

  print_results(accesses, faults, writes);
  
  return 0;
}


void print_results(long accesses, long faults, long writes){
  
  printf("\n");
  printf("\n");
  printf("================================================\n");
  printf("                   results                      \n");
  printf("================================================\n");
  printf("Number of frames:      %d \n", num_frames);
  printf("Total memory accesses: %ld\n", accesses  );
  printf("Total page faults:     %ld\n", faults    );
  printf("Total writes to disk:  %ld\n", writes    );
  printf("================================================\n");
}
 

int add_llnode(struct llnode * root, int new_val){

  struct llnode * new_node = malloc(sizeof(struct llnode));
  if(new_node == NULL){
    return -1;
  }
  new_node->val = new_val;
  new_node->next = NULL;

  struct llnode * temp;
  temp = root;

  while(temp->next != NULL){
    temp = temp->next;
  }

  temp->next = new_node;

  return 0;
}

int find_val_after(struct llnode * root, int cur_val){
  if (root == NULL)
    return -1;

  struct llnode * temp;

  temp = root;
  
  if(temp->val > cur_val){
    return temp->val;
  }
  
  while(temp->next != NULL){
    temp = temp->next;
    if(temp->val > cur_val){
      return temp->val;
    }
  }
  return -1;

}
