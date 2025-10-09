#include <stdlib.h>
#include <stdio.h> 


int main(int argc, char *argv[]){
  int ipos=-1, ival=0; 
  int *iarray = malloc(10*sizeof(int)); 

  if(iarray==NULL){ 
    printf("Memory allocation failed\n");
    return 1;
  } //
  
  if (argc == 2) ival = atoi(argv[1]);

  for (int i = 0; i<10; i++){ 
    iarray[i] = i; } 

  for (int i = 0; i<10; i++){ 
     if (ival == iarray[i]) 
     ipos = i;
}

  printf("Value %d found at position %d\n");
  free(iarray); 
  return 0; 
}
