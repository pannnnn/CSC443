#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include "library.h"

int main(int argc, char *argv[])
{
  /* Check the command line arguments */
  if (argc != 4)
  {
    printf("USAGE: %s <filename> <total bytes> <block size>\n", argv[0]);
    return 1;
  }

  /* Check total bytes and block size */
  long total_bytes = atol(argv[2]);
  long block_size = atol(argv[3]);
  if(total_bytes == 0)
    printf("Invalid total bytes: %s\n", argv[2]);
  if(block_size == 0)
    printf("Invalid block size: %s\n", argv[3]);

  /* Open the file */
  FILE *file = fopen(argv[1], "w");
  if(!file)
  {
    printf("Could not open %s\n", argv[1]);
    return 1;
  }

  /* Allocate buffer */
  char *buffer = (char *)calloc(1, block_size);
  long bytes_left = total_bytes;

  /* Record the start time */
  struct timeb t;
  ftime(&t);
  long start_time = t.time * 1000 + t.millitm;

  /* Write randomly generated letters into file */
  random_array(buffer, block_size);
  size_t bytes_write;
  while(bytes_left > block_size)
  {
    bytes_write = fwrite(buffer, 1, block_size, file);
    if(bytes_write < block_size){
      fprintf(stderr, "Byte write failed");
      exit(ferror(file));
    }
    /* Write to disk immediately (skip buffering) */
    fflush(file);
    bytes_left -= block_size;
  }
  bytes_write = fwrite(buffer, 1, bytes_left, file);
  if(bytes_write < bytes_left){
    fprintf(stderr, "Byte write failed");
    exit(ferror(file));
  }
  fclose(file);

  /* Record the end time and report time consumed */
  ftime(&t);
  long end_time = t.time * 1000 + t.millitm;
  long milliseconds = end_time - start_time;   
  printf("%ld\n", milliseconds);   

  return 0;
}
