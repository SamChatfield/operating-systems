#include "linkedlist.h"
#include <stdio.h>
#include <assert.h>

//defines
//debug prints: set DEBUG to 0 to remove the debug prints from the output
#define DEBUG 0
#define debug_print(...) \
  do { if(DEBUG) fprintf(stderr, __VA_ARGS__); } while(0)


// --- MAIN ---
int main(int argc, char** argv){
  list l;
  init(&l);
  
  //testcase
  debug_print("prepend\n");
  prepend(&l, 1);
  print_list(&l); //1

  printf("get: %d\n", get(&l, 0));
  
  debug_print("append\n");
  append(&l, 3);
  print_list(&l); //1 3

  debug_print("insert\n");
  insert(&l, 0, 2);
  print_list(&l); //1 2 3

  debug_print("insert\n");
  insert(&l, 2, 4);
  print_list(&l); //1 2 3 4
  
  debug_print("remove\n");
  remove_element(&l, 2);
  print_list(&l); //1 2 4
  debug_print("remove\n");
  remove_element(&l, 0);
  print_list(&l); //2 4

  debug_print("insert\n");
  assert(insert(&l, 10, 30) == -1); //this should fail
  print_list(&l); //2 4
  
  debug_print("remove\n");
  remove_element(&l, 0);
  print_list(&l); //4
  
  debug_print("remove\n");
  remove_element(&l, 0);
  print_list(&l); //empty list

  debug_print("remove\n");
  assert(remove_element(&l, 0) == -1); //this should fail
  print_list(&l); //empty list
  destroy(&l);
  return 0;
}
