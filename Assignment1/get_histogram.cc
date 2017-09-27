#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "library.h"

int main(int argc, char *argv[])
{
  /* Check the command line arguments */
  if (argc != 3)
  {
    printf("USAGE: %s <filename> <blocksize>\n", argv[0]);
    return 1;
  }

  /* Check total bytes and block size */
  long block_size = atol(argv[2]);
  if(block_size == 0)
    printf("Invalid block size: %s\n", argv[2]);

  /* Open the file */
  FILE *file = fopen(argv[1], "r");
  if(!file)
  {
    printf("Could not open %s\n", argv[1]);
    return 1;
  }

  /* Initialize the parameters for the function call */
  long hist[26];
  long milliseconds;
  long filelen;
  FILE *file_ptr = fopen(argv[1], "r");
    
  /* Compute the histogram and check the result */
  int ret = get_histogram(file_ptr, hist, block_size, &milliseconds, &filelen);
  assert(ret>=0);

  /* Print out the result as requested in assignment */
  for(int i=0; i < 26; i++) {
      printf("%c : %ld\n", 'A' + i, hist[i]);
  }
  printf("BLOCK SIZE  %ld bytes\n", block_size);
  printf("TOTAL BYTES %ld bytes\n", filelen);
  printf("Time %ld milliseconds.\n", milliseconds);
}
