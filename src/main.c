#include "stdlib.h"
#include "stdio.h"


/* ################################ Globals ################################ */


/**
 * @brief DNA sequence string.
 */
char* string = NULL;

/**
 * @brief Size of input string.
 */
int size_string = 0;


/* ################################ Helpers ################################ */


/**
 * @brief Frees all the allocated memory in all nodes.
 */
void free_program_memory() {
  free(string);
}


/* ######################### Suffix Tree Algorithm ########################## */



/* ################################# Funcs ################################# */


/**
 * @brief Reads input from stdin and builds structures to be used.
 */
void read_input() {
  /* Used as parameters in getline to store input and size of input */ 
  size_t len = 0;
  char* input = NULL;

  /* Reads size of dna string and the dna sequence */
  getline(&input, &len, stdin);

  /* Allocates memory for the string */
  sscanf(input, "%d %*s", &size_string);
  string = (char*) calloc(size_string + 1, sizeof(char));

  /* Stores dna sequence in the string */
  sscanf(input, "%*d %s", string);

  free(input);
}

/**
 * @brief Main driver code.
 * 
 * @return int 0 for success or 1 for error
 */
int main() {

  /* Reads input from the stdin and builds structures */
  read_input();

  /* Cleans up the program by freeing all the allocated memory */
  free_program_memory();

  exit(0);
}
